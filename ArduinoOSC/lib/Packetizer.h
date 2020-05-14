#pragma once
#ifndef ARDUINO_OSC_PACKETIZER_H
#define ARDUINO_OSC_PACKETIZER_H

#ifdef TEENSYDUINO
#ifndef TEENSYDUINO_DIRTY_STL_ERROR_SOLUTION
#define TEENSYDUINO_DIRTY_STL_ERROR_SOLUTION
extern "C"
{
    int _getpid() { return -1; }
    int _kill(int pid, int sig) { return -1; }
    int _write() { return -1; }
    void *__exidx_start __attribute__((__visibility__ ("hidden")));
    void *__exidx_end __attribute__((__visibility__ ("hidden")));
}
// DIRTY for TEENSYDUINO compile...
// copied from https://github.com/gcc-mirror/
namespace std
{
    void __throw_bad_alloc() { _GLIBCXX_THROW_OR_ABORT(bad_alloc()); }
    void __throw_length_error(const char* __s __attribute__((unused))) { _GLIBCXX_THROW_OR_ABORT(length_error(_(__s))); }
    void __throw_bad_function_call() { _GLIBCXX_THROW_OR_ABORT(bad_function_call()); }
    // void _Rb_tree_decrement(std::_Rb_tree_node_base* a) {}
    // void _Rb_tree_insert_and_rebalance(bool, std::_Rb_tree_node_base*, std::_Rb_tree_node_base*, std::_Rb_tree_node_base&) {}
}
#endif // TEENSYDUINO_DIRTY_STL_ERROR_SOLUTION
#endif // TEENSYDUINO


#if defined(__AVR__) || defined(ARDUINO_spresense_ast)
#include "RingBuffer.h"
#else
#include <vector>
#include <queue>
// #include <map>
#include <functional>
#endif // __AVR__

namespace arduino {
namespace osc { // for ArduinoOSC
namespace packetizer {

    static constexpr uint8_t INDEX_OFFSET_INDEX = 1;
    static constexpr uint8_t INDEX_OFFSET_DATA = 2;
    static constexpr uint8_t INDEX_OFFSET_CRC_ESCAPE_FROM_END = 2;
    static constexpr uint8_t N_HEADER_SIZE = 2;
    static constexpr uint8_t N_FOOTER_SIZE = 1; // footer is not bufferred
    static constexpr uint8_t N_HEADER_FOOTER_SIZE = N_HEADER_SIZE + N_FOOTER_SIZE;

    struct endp {}; // for end of packet sign

    static uint8_t crc8(const uint8_t* data, size_t size)
    {
        uint8_t result = 0xFF;
        for (result = 0; size != 0; --size)
        {
            result ^= *data++;
            for (size_t i = 0 ; i < 8; ++i)
            {
                if (result & 0x80)
                {
                    result <<= 1;
                    result ^= 0x85; // x8 + x7 + x2 + x0
                }
                else
                    result <<= 1;
            }
        }
        return result;
    }


#if defined(__AVR__) || defined(ARDUINO_spresense_ast)

    template <uint8_t N_PACKET_DATA_SIZE, uint8_t START_BYTE = 0xC1>
    class Encoder_
    {
        // using Encoder = Encoder_;
        using Buffer = RingBuffer<uint8_t, N_PACKET_DATA_SIZE>;
        using EscapeBuffer = RingBuffer<uint8_t, N_PACKET_DATA_SIZE>;

#else

    template <uint8_t START_BYTE = 0xC1>
    class Encoder_
    {
        using Buffer = std::vector<uint8_t>;
        using EscapeBuffer = std::queue<uint8_t>;

#endif

        static constexpr uint8_t FINISH_BYTE {START_BYTE + 1};
        static constexpr uint8_t ESCAPE_BYTE {START_BYTE + 2};
        static constexpr uint8_t ESCAPE_MASK {0x20};

        Buffer buffer;

    public:

        Encoder_(const uint8_t idx = 0) { init(idx); }


        // get packing info
        size_t size() const { return buffer.size(); }
        const uint8_t* data() const { return buffer.data(); }

        void init(const uint8_t index = 0)
        {
            buffer.clear();
            append((uint8_t)START_BYTE, false);
            append((uint8_t)index);
        }

        // ---------- pack w/ insertion operator ----------

        const endp& operator<< (const endp& e)
        {
            footer();
            return e; // dummy
        }
        Encoder_& operator<< (const uint8_t arg)
        {
            append(arg);
            return *this;
        }

        // ---------- pack w/ variadic arguments ----------

        template <typename ...Rest>
        void pack(const uint8_t first, const Rest&& ...args)
        {
            append((uint8_t)first);
            pack(args...);
        }
        void pack()
        {
            footer();
        }

        // ---------- pack w/ data pointer and size ----------

        void pack(const uint8_t* const sbuf, const uint8_t size, const uint8_t index = 0)
        {
            init(index);
            append((uint8_t*)sbuf, size);
            footer();
        }

    private:

        void append(const uint8_t* const data, const uint8_t size, const bool b_escape = true)
        {
            if (b_escape)
            {
                EscapeBuffer escapes;
                for (uint8_t i = 0; i < size; ++i)
                    if (is_escape_byte(data[i]))
                        escapes.push(i);

                if (escapes.empty())
                    for (uint8_t i = 0; i < size; ++i) buffer.push_back(data[i]);
                else
                {
                    size_t start = 0;
                    while (!escapes.empty())
                    {
                        const uint8_t& idx = escapes.front();
                        append(data + start, idx - start);
                        append(data[idx], true);
                        start = idx + 1;
                        escapes.pop();
                    }
                    if (start < size) append(data + start, size - start);
                }
            }
            else
                for (uint8_t i = 0; i < size; ++i) buffer.push_back(data[i]);
        }

        void append(const uint8_t data, const bool b_escape = true)
        {
            if (b_escape && is_escape_byte(data))
            {
                buffer.push_back((uint8_t)ESCAPE_BYTE);
                buffer.push_back((uint8_t)(data ^ ESCAPE_MASK));
            }
            else
                buffer.push_back(data);
        }

        void footer()
        {
            append(crc8(buffer.data(), buffer.size()));
            append(FINISH_BYTE, false);
        }

        bool is_escape_byte(const uint8_t d) const
        {
            return ((d == START_BYTE) || (d == ESCAPE_BYTE) || (d == FINISH_BYTE));
        }

    };


#if defined(__AVR__) || defined(ARDUINO_spresense_ast)

    template <uint8_t N_PACKET_QUEUE_SIZE, uint8_t N_PACKET_DATA_SIZE, uint8_t N_CALLBACK_SIZE = 8, uint8_t START_BYTE = 0xC1>
    class Decoder_
    {
        using Buffer = RingBuffer<uint8_t, N_PACKET_DATA_SIZE>;
        typedef void (*callback_t)(const uint8_t* data, const uint8_t size);
        struct Map { uint8_t key; callback_t func; };
        using PacketQueue = RingBuffer<Buffer, N_PACKET_QUEUE_SIZE>;
        using CallbackMap = RingBuffer<Map, N_CALLBACK_SIZE>;

#else

    template <uint8_t N_PACKET_QUEUE_SIZE, uint8_t START_BYTE = 0xC1>
    class Decoder_
    {
        using Buffer = std::vector<uint8_t>;
        using callback_t = std::function<void(const uint8_t* data, const uint8_t size)>;
        struct Map { uint8_t key; callback_t func; };
        using PacketQueue = std::queue<Buffer>;
        using CallbackMap = std::vector<Map>;

#endif // __AVR__

        static constexpr uint8_t FINISH_BYTE {START_BYTE + 1};
        static constexpr uint8_t ESCAPE_BYTE {START_BYTE + 2};
        static constexpr uint8_t ESCAPE_MASK {0x20};

        Buffer buffer;
        PacketQueue packets;
        CallbackMap callbacks;

        bool b_parsing {false};
        bool b_escape {false};

        uint32_t err_count {0};

    public:

        using CallbackType = callback_t;
        // TODO: std::map / unordered_map compile error for teensy in Arduino IDE...
        void subscribe(const uint8_t index, const callback_t& func) { callbacks.push_back({index, func}); }

        size_t available() const { return packets.size(); }
        uint8_t index() const { return packets.front()[INDEX_OFFSET_INDEX]; }
        uint8_t size() const { return packets.front().size() - N_HEADER_FOOTER_SIZE; }
        uint8_t data(const uint8_t i) const { return data()[i]; }
        const uint8_t* data() const { return packets.front().data() + INDEX_OFFSET_DATA; }

        void pop() { packets.pop(); }

        void feed(const uint8_t* const data, const size_t size)
        {
            for (size_t i = 0; i < size; ++i) feed(data[i]);
        }

        void feed(const uint8_t d)
        {
            if (d == START_BYTE)
            {
                reset();
                buffer.push_back(d);
                b_parsing = true;
            }
            else if (b_parsing)
            {
                if (d == FINISH_BYTE)
                    decode();
                else if (b_parsing)
                    buffer.push_back(d);
            }

            // TODO: std::map / unordered_map compile error...
            // if (available() && !callbacks.empty())
            // {
            //     auto it = callbacks.find(index());
            //     if (it != callbacks.end()) { it->second(data(), size()); pop(); }
            // }

            if (!available()) return;

            for (auto& c : callbacks)
            {
                if (available() && (c.key == index()))
                {
                    c.func(data(), size());
                    pop();
                }
            }
        }

        bool isParsing() const { return b_parsing; }

        uint32_t errors() const { return err_count; }

        void reset()
        {
            buffer.clear();
            b_parsing = false;
        }

    private:

        void decode()
        {
            if (isCrcMatched())
            {
                for (auto it = buffer.begin(); it != buffer.end(); ++it)
                {
                    if (*it == ESCAPE_BYTE)
                    {
                        it = buffer.erase(it);
                        *it = *it ^ ESCAPE_MASK;
                    }
                }
                packets.push(buffer);
            }
            else
                ++err_count;

            reset();

            if (available() > N_PACKET_QUEUE_SIZE) pop();
        }

        bool isCrcMatched()
        {
            uint8_t crc_received = buffer.back();
            uint8_t crc_offset_size = 1;
            if (*(buffer.end() - INDEX_OFFSET_CRC_ESCAPE_FROM_END) == ESCAPE_BYTE) // before CRC byte can be ESCAPE_BYTE only if CRC is escaped
            {
                crc_received ^= ESCAPE_MASK;
                crc_offset_size = 2;
            }

            uint8_t crc = crc8(buffer.data(), buffer.size() - crc_offset_size);
            return (crc == crc_received);
        }

    };

#if defined(__AVR__) || defined(ARDUINO_spresense_ast)
    using Encoder = Encoder_<64>;
    using Decoder = Decoder_<2, 64>;
#else
    using Encoder = Encoder_<>;
    using Decoder = Decoder_<4>;
#endif
    using CallbackType = Decoder::CallbackType;

} // packetizer
} // osc // for ArduinoOSC
} // arduino

// namespace Packetizer = arduino::packetizer;

#endif // ARDUINO_OSC_PACKETIZER_H
