#pragma once

#ifndef ARDUINOOSC_OSCDECODER_H
#define ARDUINOOSC_OSCDECODER_H

#include <Arduino.h>
#include "util/DebugLog/DebugLog.h"
#include "OscTypes.h"
#include "OscMessage.h"

namespace arduino {
namespace osc {
    namespace message {

        class Decoder {
            using MessageIterator = MessageQueue::iterator;
            MessageQueue messages;
            MessageIterator it_messages;

        public:
            Decoder()
            : messages(), it_messages(messages.begin()) {}

            Decoder(const void* ptr, const size_t sz)
            : messages(), it_messages(messages.begin()) {
                init(ptr, sz);
            }

            bool init(const void* ptr, const size_t sz) {
                messages.clear();
                if ((sz % 4) == 0) {
                    if (parse((const char*)ptr, (const char*)ptr + sz, TimeTag::immediate())) {
                        it_messages = messages.begin();
                        return true;
                    }
                }
                LOG_ERROR(F("parse message failed"));
                return false;
            }

            Message* decode() {
                if (messages.empty()) {
                    LOG_ERROR(F("message is empty"));
                    return nullptr;
                }
                if (it_messages == messages.end()) {
                    LOG_ERROR(F("no more message to decode"));
                    return nullptr;
                }

                return &*it_messages++;
            }

        private:
            bool parse(const char* beg, const char* end, const TimeTag& time_tag) {
                if (beg >= end) {
                    LOG_ERROR(F("data ptr should be begin > end but it was:"), beg, ">=", end);
                    return false;
                }

                if (*beg == '#') {
                    if ((end - beg >= 20) && (memcmp(beg, "#bundle\0", 8) == 0)) {
                        TimeTag tt(bytes2pod<uint64_t>(beg + 8));
                        const char* pos = beg + 16;
                        do {
                            uint32_t sz = bytes2pod<uint32_t>(pos);
                            pos += 4;
                            if ((sz & 3) != 0 || pos + sz > end || pos + sz < pos) {
                                LOG_ERROR(F("bundle data structure was corrupted"));
                                return false;
                            }
                            parse(pos, pos + sz, tt);
                            pos += sz;
                        } while (pos != end);
                    } else {
                        LOG_ERROR(F("bundle header was corrupted"));
                        return false;
                    }
                } else
                    messages.push_back(Message(beg, end - beg, time_tag));

                return true;
            }
        };

    }  // namespace message
}  // namespace osc
}  // namespace arduino

using OscDecoder = arduino::osc::message::Decoder;

#endif  // ARDUINOOSC_OSCDECODER_H
