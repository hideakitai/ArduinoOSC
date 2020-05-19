#pragma once

#ifndef ARDUINOOSC_OSCDECODER_H
#define ARDUINOOSC_OSCDECODER_H

#include <Arduino.h>
#include "OscTypes.h"
#include "OscMessage.h"

namespace arduino {
namespace osc {
namespace message {

    class Decoder
    {
        MessageQueue messages;
        MessageIterator it_messages;

    public:

        Decoder() { }

        Decoder(const void *ptr, const size_t sz)
        {
            init(ptr, sz);
        }

        bool init(const void *ptr, const size_t sz)
        {
            messages.clear();
            if ((sz % 4) == 0)
            {
                if (parse((const char*)ptr, (const char *)ptr + sz, TimeTag::immediate()))
                {
                    it_messages = messages.begin();
                    return true;
                }
            }
            return false;
        }

        Message* decode()
        {
            if (!messages.empty() && (it_messages != messages.end()))
                return &*it_messages++;
            return nullptr;
        }

    private:

        bool parse(const char *beg, const char *end, const TimeTag& time_tag)
        {
            if (beg == end) return false;

            if (*beg == '#')
            {
                if ((end - beg >= 20) && (memcmp(beg, "#bundle\0", 8) == 0))
                {
                    TimeTag tt(bytes2pod<uint64_t>(beg + 8));
                    const char *pos = beg + 16;
                    do
                    {
                        uint32_t sz = bytes2pod<uint32_t>(pos);
                        pos += 4;
                        if ((sz & 3) != 0 || pos + sz > end || pos+sz < pos)
                            return false;
                        else
                        {
                            parse(pos, pos + sz, tt);
                            pos += sz;
                        }
                    } while (pos != end);
                }
                else
                    return false;
            }
            else
                messages.push_back(Message(beg, end - beg, time_tag));

            return true;
        }
    };

} // namespace message
} // namespace osc
} // namespac arduino


using OscDecoder = arduino::osc::message::Decoder;


#endif // ARDUINOOSC_OSCDECODER_H
