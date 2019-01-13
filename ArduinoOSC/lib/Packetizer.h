#pragma once
#ifndef PACKETIZER_IMPL_H
#define PACKETIZER_IMPL_H

#ifdef __AVR__
#include "RingBuffer.h"
#else
#include <vector>
#include <queue>
// #include <map>
#include <functional>
#endif // __AVR__

namespace Packetizer
{
    static constexpr uint8_t START_BYTE  = 0x7D;
    static constexpr uint8_t FINISH_BYTE = 0x7E;
    static constexpr uint8_t ESCAPE_BYTE = 0x7F;
    static constexpr uint8_t ESCAPE_MASK = 0x20;

    static constexpr uint8_t INDEX_OFFSET_INDEX = 1;
    static constexpr uint8_t INDEX_OFFSET_DATA = 2;
    static constexpr uint8_t INDEX_OFFSET_CRC_ESCAPE_FROM_END = 2;
    static constexpr uint8_t N_HEADER_SIZE = 2;
    static constexpr uint8_t N_FOOTER_SIZE = 1; // footer is not bufferred
    static constexpr uint8_t N_HEADER_FOOTER_SIZE = N_HEADER_SIZE + N_FOOTER_SIZE;

    struct endp {}; // for end of packet sign

    uint8_t crc8(const uint8_t* data, size_t size)
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


#ifdef __AVR__
    template <uint8_t N_PACKET_DATA_SIZE>
    class Packer_
    {
        using Packer = Packer_;
        using Buffer = RingBuffer<uint8_t, N_PACKET_DATA_SIZE>;
        using EscapeBuffer = RingBuffer<uint8_t, N_PACKET_DATA_SIZE>;
    public:
        Packer_(uint8_t idx = 0) { init(idx); }
#else
    class Packer
    {
        using Buffer = std::vector<uint8_t>;
        using EscapeBuffer = std::queue<uint8_t>;
    public:
        Packer(uint8_t idx = 0) { init(idx); }
#endif // __AVR__

        // get packing info
        size_t size() const { return buffer.size(); }
        const uint8_t* data() const { return buffer.data(); }

        void init(uint8_t index = 0)
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
        Packer& operator<< (const uint8_t arg)
        {
            append(arg);
            return *this;
        }

        // ---------- pack w/ variadic arguments ----------

        template <typename ...Rest>
        void pack(uint8_t first, Rest&& ...args)
        {
            append((uint8_t)first);
            pack(args...);
        }
        void pack()
        {
            footer();
        }

        // ---------- pack w/ data pointer and size ----------

        void pack(const uint8_t* const sbuf, uint8_t size, uint8_t index = 0)
        {
            init(index);
            append((uint8_t*)sbuf, size);
            footer();
        }

    private:

        void append(const uint8_t* const data, uint8_t size, bool b_escape = true)
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

        void append(uint8_t data, bool b_escape = true)
        {
            if (b_escape && is_escape_byte(data))
            {
                buffer.push_back(ESCAPE_BYTE);
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

        bool is_escape_byte(uint8_t d) const
        {
            return ((d == START_BYTE) || (d == ESCAPE_BYTE) || (d == FINISH_BYTE));
        }

        Buffer buffer;
    };


#ifdef __AVR__
    template <uint8_t N_PACKET_QUEUE_SIZE, uint8_t N_PACKET_DATA_SIZE, uint8_t N_CALLBACK_SIZE = 8>
    class Unpacker_
    {
        using Buffer = RingBuffer<uint8_t, N_PACKET_DATA_SIZE>;
        typedef void (*CallbackType)(const uint8_t* data, uint8_t size);
        struct Map { uint8_t key; CallbackType func; };
        using PacketQueue = RingBuffer<Buffer, N_PACKET_QUEUE_SIZE>;
        using CallbackMap = RingBuffer<Map, N_CALLBACK_SIZE>;
#else
    template <uint8_t N_PACKET_QUEUE_SIZE>
    class Unpacker_
    {
        using Buffer = std::vector<uint8_t>;
        using CallbackType = std::function<void(const uint8_t* data, uint8_t size)>;
        struct Map { uint8_t key; CallbackType func; };
        using PacketQueue = std::queue<Buffer>;
        using CallbackMap = std::vector<Map>;
#endif // __AVR__

    public:

        // TODO: std::map / unordered_map compile error for teensy in Arduino IDE...
        void subscribe(uint8_t index, CallbackType func) { callbacks.push_back({index, func}); }

        size_t available() const { return packets.size(); }
        uint8_t index() const { return packets.front()[INDEX_OFFSET_INDEX]; }
        uint8_t size() const { return packets.front().size() - N_HEADER_FOOTER_SIZE; }
        uint8_t data(uint8_t i) const { return data()[i]; }
        const uint8_t* data() const { return packets.front().data() + INDEX_OFFSET_DATA; }

        void pop() { packets.pop(); }

        void feed(const uint8_t* const data, size_t size)
        {
            for (size_t i = 0; i < size; ++i) feed(data[i]);
        }

        void feed(uint8_t d)
        {
            if (!b_processing)
            {
                if (d == START_BYTE)
                {
                    reset();
                    buffer.push_back(d);
                    b_processing = true;
                }
            }
            else
            {
                if (d == FINISH_BYTE) check_crc();
                else                  buffer.push_back(d);
            }
            // TODO: std::map / unordered_map compile error...
            // if (available() && !callbacks.empty())
            // {
            //     auto it = callbacks.find(index());
            //     if (it != callbacks.end()) { it->second(data(), size()); pop(); }
            // }
            if (!callbacks.empty())
            {
                for (auto& c : callbacks)
                {
                    if (available())
                    {
                        if (c.key == index())
                        {
                            c.func(data(), size());
                            pop();
                        }
                        else if (available() > N_PACKET_QUEUE_SIZE)
                        {
                            // TODO: discard if unpacker have callbacks?
                            // TODO: that is select callback or manual switch-case / if-else
                            pop();
                        }
                    }
                }
            }
        }

    private:

        void check_crc()
        {
            uint8_t crc_received = buffer.back();
            uint8_t crc_offset_size = 1;
            if (*(buffer.end() - INDEX_OFFSET_CRC_ESCAPE_FROM_END) == ESCAPE_BYTE) // before CRC byte can be ESCAPE_BYTE only if CRC is escaped
            {
                crc_received ^= ESCAPE_MASK;
                crc_offset_size = 2;
            }

            uint8_t crc = crc8(buffer.data(), buffer.size() - crc_offset_size);
            if (crc == crc_received)
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
            reset();
        }

        void reset()
        {
            buffer.clear();
            b_processing = false;
        }

        bool b_processing {false};
        bool b_escape {false};

        Buffer buffer;
        PacketQueue packets;
        CallbackMap callbacks;
    };

#ifdef __AVR__
    using Packer = Packer_<64>;
    using Unpacker = Unpacker_<2, 64>;
#else
    using Unpacker = Unpacker_<4>;
#endif // __AVR__
}


#endif // PACKETIZER_IMPL_H
