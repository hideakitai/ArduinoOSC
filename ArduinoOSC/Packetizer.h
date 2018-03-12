#pragma once
#include <queue>

namespace Packetizer
{
    enum class State { Start, Index, Size, Data, Checksum };
    enum class Checker { None, Sum, CRC8 };

    const uint8_t START_BYTE = 0x7E;
    const uint8_t ESCAPE_BYTE = 0x7D;
    const uint8_t ESCAPE_MASK = 0x20;
    const uint16_t ESCAPE_MARKER = 0xFFFF;

    const uint16_t SEND_BUFFER_SIZE = 128;
    const uint16_t READ_BUFFER_SIZE = 128;
}

namespace CRC
{
    static uint8_t getCRC8(const uint8_t *buff, size_t size)
    {
        uint8_t* p = (uint8_t*)buff;
        uint8_t result = 0xFF;

        for (result = 0 ; size != 0 ; size--)
        {
            result ^= *p++;

            for (size_t i = 0 ; i < 8; i++)
            {
                if (result & 0x80)
                {
                    result <<= 1; result ^= 0x85; // x8 + x7 + x2 + x0
                }
                else
                {
                    result <<= 1;
                }
            }
        }
        return result;
    }


    // static uint16_t getCRC16(const uint8_t* buff, size_t size)
    // {
    //     uint8_t* data = (uint8_t*)buff;
    //     uint16_t result = 0xFFFF;

    //     for (size_t i = 0; i < size; ++i)
    //     {
    //         result ^= data[i];
    //         for (size_t j = 0; j < 8; ++j)
    //         {
    //             if (result & 0x01) result = (result >> 1) ^ 0xA001;
    //             else result >>= 1;
    //         }
    //     }
    //     return result;
    // }
}

namespace std
{
  void __throw_bad_alloc()
  {
    Serial.println("Unable to allocate memory");
  }

  void __throw_length_error(char const* e)
  {
    Serial.print("Length Error :");
    Serial.println(e);
  }
}


namespace Packetizer
{
    class Packer
    {
    public:

        Packer(Checker m = Checker::CRC8) : mode(m) {}
        ~Packer() {}

        void setCheckMode(Checker m) { mode = m; }

		uint8_t* data() { return pack_buffer; }

        size_t size() { return count; }


        const uint8_t* pack(uint8_t* sbuf, uint8_t size, const uint8_t& index = 0)
        {
            memset(pack_buffer, 0, READ_BUFFER_SIZE);
            count = 0;

            append((uint8_t)START_BYTE, false);
            append((uint8_t)index);
            append((uint8_t)size);
            append((uint8_t*)sbuf, size);

            if (mode == Checker::Sum)
            {
                uint8_t sum = (uint8_t)START_BYTE + index + (uint8_t)size;
                for (size_t i = 0; i < size; ++i) sum += (uint8_t)sbuf[i];
                append(sum);
            }
            else if (mode == Checker::CRC8)
            {
                append(CRC::getCRC8((uint8_t*)sbuf, size));
            }

            return data();
        }

    protected:

        void append(const uint8_t* const data, const uint16_t& size, bool isEscape = true)
        {
            if (isEscape)
            {
                std::queue<uint16_t> escapes;
                for (size_t i = 0; i < size; ++i)
                    if ((data[i] == START_BYTE) || (data[i] == ESCAPE_BYTE))
                        escapes.push(i);

                if (escapes.empty())
                {
                    for (size_t i = 0; i < size; ++i)
                    {
                        pack_buffer[count++] = data[i];
                    }
                }
                else
                {
                    size_t start = 0;
                    while (!escapes.empty())
                    {
                        const size_t& idx = escapes.front();
                        append(data + start, idx - start);
                        append(data[idx], true);
                        start = idx + 1;
                        escapes.pop();
                    }
                    if (start < size) append(data + start, size - start);
                }
            }
            else
            {
                for (size_t i = 0; i < size; ++i)
                {
                    pack_buffer[count++] = data[i];
                }
            }
        }


        void append(const uint8_t& data, bool isEscape = true)
        {
            if (isEscape && ((data == START_BYTE) || (data == ESCAPE_BYTE)))
            {
                pack_buffer[count++] = ESCAPE_BYTE;
                pack_buffer[count++] = (uint8_t)(data ^ ESCAPE_MASK);
            }
            else
            {
                pack_buffer[count++] = data;
            }
        }

    private:

        Checker mode;

        uint8_t pack_buffer[SEND_BUFFER_SIZE];
        size_t count;
    };

}


namespace Packetizer
{
    class Unpacker
    {
    public:

        Unpacker(Checker m = Checker::CRC8)
        : r_buffer(), state(State::Start), b_escape(false) , sum(0), count(0), mode(m)
        {}

        ~Unpacker() {}

        size_t available() { return _readBuffer.size(); }
        uint8_t index() { return _readBuffer.front().index; }
        uint8_t size() { return _readBuffer.front().size; }
        uint8_t* data() { return _readBuffer.front().sbuf; }

        void setCheckMode(Checker m) { mode = m; }

        void pop() { _readBuffer.pop(); }

        void feed(const uint8_t* const data, const size_t size)
        {
            for (size_t i = 0; i < size; ++i) feed(data[i]);
        }

        void feed(uint8_t d)
        {
            uint16_t buff = append(d);
            if (buff == ESCAPE_MARKER) return;

            uint8_t data = (uint8_t)(buff & 0x00FF);

            if ((mode == Checker::Sum) && (state != State::Checksum)) sum += data;

            switch(state)
            {
                case State::Start:
                {
                    reset();
                    if (data == (uint8_t)START_BYTE) state = State::Index;
                    if (mode == Checker::Sum) sum += data;
                    break;
                }
                case State::Index:
                {
                    r_buffer.index = data;
                    state = State::Size;
                    break;
                }
                case State::Size:
                {
                    r_buffer.size = data;
                    state = State::Data;
                    break;
                }
                case State::Data:
                {
                    r_buffer.write(&data, 1);
                    if (++count >= r_buffer.size) state = State::Checksum;
                    break;
                }
                case State::Checksum:
                {
                    if ((mode == Checker::Sum) && (sum == data))
                    {
                        _readBuffer.push(r_buffer);
                    }
                    else if (mode == Checker::CRC8)
                    {
                        uint8_t crc8 = CRC::getCRC8((uint8_t*)r_buffer.sbuf, r_buffer.size);
                        if (crc8 == data) _readBuffer.push(r_buffer);
                    }
                    reset();
                    break;
                }
                default:
                {
                    reset();
                    break;
                }
            }
        }

    protected:

        uint16_t append(const uint8_t& data)
        {
            uint16_t byteIn = (uint16_t)data;

            if (byteIn == (uint16_t)ESCAPE_BYTE)
            {
                byteIn = ESCAPE_MARKER;
                b_escape = true;
            }
            else
            {
                if (b_escape)
                {
                    byteIn = (uint16_t)(byteIn ^ ESCAPE_MASK);
                    b_escape = false;
                }
            }
            return byteIn;
        }

        void reset()
        {
            r_buffer.clear();
            sum = count = 0;
            state = State::Start;
        }

    protected:

        struct Buffer
        {
            uint8_t index;
            uint8_t size;
            uint8_t sbuf[READ_BUFFER_SIZE];
            uint8_t count;

            void write(uint8_t* data, uint8_t size)
            {
                memcpy(sbuf + count, data, size);
                count += size;
            }

            void clear() { index = size = count = 0; memset(sbuf, 0, READ_BUFFER_SIZE); }
        };

        Buffer r_buffer;

        State state;
        bool b_escape;

        uint8_t sum;
        uint8_t count;

        Checker mode;
        std::queue<Buffer> _readBuffer;
    };

}
