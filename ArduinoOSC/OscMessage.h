#pragma once

#ifndef ARDUINOOSC_OSCMESSAGE_H
#define ARDUINOOSC_OSCMESSAGE_H

#include <Arduino.h>
#include "OscTypes.h"
#include "OscUtil.h"

namespace arduino {
namespace osc {
namespace message {

#ifdef ARDUINOOSC_DISABLE_STL
    using namespace arx;
#else
    using namespace std;
#endif

    class Message
    {
        TimeTag time_tag;
        String address_str;
        String type_tags;
        Storage storage;
        ArgumentQueue arguments;
#ifndef ARDUINO_AVR_UNO // TODO: dirty...
        bool valid = false;
#endif

        String remote_ip;
        uint16_t remote_port;

    public:

        Message()
        {
            clear();
        }
        Message(const String &s, const TimeTag tt = TimeTag::immediate())
        : time_tag(tt), address_str(s)
        {
            init(s, tt);
        }
        Message(const void *ptr, const size_t sz, const TimeTag tt = TimeTag::immediate())
        {
#ifndef ARDUINO_AVR_UNO // TODO: dirty...
            valid = buildFromRawData(ptr, sz);
#else
            buildFromRawData(ptr, sz);
#endif
            time_tag = tt;
        }
        Message(const String& ip, const uint16_t port, const String& addr)
        : remote_ip(ip) , remote_port(port)
        {
            init(addr);
        }

        virtual ~Message() {}


        Message& init(const String& addr, const TimeTag tt = TimeTag::immediate())
        {
            clear();
            address_str = addr;
            time_tag = tt;
            return *this;
        }

        bool match(const String& pattern, const bool full = true) const
        {
            if (full)
                return fullPatternMatch(pattern.c_str(), address_str.c_str());
            else
                return partialPatternMatch(pattern.c_str(), address_str.c_str());
        }

        void encode(Storage &s, const bool write_size = false) const
        {
            size_t l_addr = address_str.length() + 1;
            size_t l_type = type_tags.length() + 2;
            if (write_size)
            {
                uint32_t sz = uint32_t(ceil4(l_addr) + ceil4(l_type) + ceil4(storage.size()));
                pod2bytes<uint32_t>(sz, s.getBytes(4));
            }

            strcpy(s.getBytes(l_addr), address_str.c_str());
            strcpy(s.getBytes(l_type), ("," + type_tags).c_str());
            if (storage.size())
                memcpy(s.getBytes(storage.size()), const_cast<Storage&>(storage).begin(), storage.size());
        }

        bool available() const
        {
#ifndef ARDUINO_AVR_UNO // TODO: dirty...
            return valid;
#else
            return true;
#endif
        }

        void clear()
        {
            address_str = type_tags = String("");
            storage.clear();
            arguments.clear();
            time_tag = TimeTag::immediate();
        }


        ////////////////////////////////////////////
        // ---------- adding arguments ---------- //
        ////////////////////////////////////////////

        // TODO: reference, move....
        template <typename T>
        Message& push(const T& t);
        Message& push(const char* c) { return pushString(String(c)); }

        Message& pushBool(const bool b)
        {
            type_tags += (char)(b ? TYPE_TAG_TRUE : TYPE_TAG_FALSE);
            arguments.push_back(make_pair(storage.size(), storage.size()));
            return *this;
        }
        Message& pushInt32(const int32_t i) { return pushPod(TYPE_TAG_INT32, i); }
        Message& pushInt64(const int64_t h) { return pushPod(TYPE_TAG_INT64, h); }
        Message& pushFloat(const float f) { return pushPod(TYPE_TAG_FLOAT, f); }
        Message& pushDouble(const double d) { return pushPod(TYPE_TAG_DOUBLE, d); }
        Message& pushString(const String &s)
        {
            type_tags += (char)TYPE_TAG_STRING;
            arguments.push_back(make_pair(storage.size(), s.length() + 1));
            strcpy(storage.getBytes(s.length() + 1), s.c_str());
            return *this;
        }
        Message& pushBlob(const Blob& b) { return pushBlob(b.data(), b.size()); }
        Message& pushBlob(const void *ptr, const size_t num_bytes)
        {
            type_tags += (char)TYPE_TAG_BLOB;
            arguments.push_back(make_pair(storage.size(), num_bytes + 4));
            pod2bytes<int32_t>((int32_t)num_bytes, storage.getBytes(4));
            if (num_bytes) memcpy(storage.getBytes(num_bytes), ptr, num_bytes);
            return *this;
        }


        ////////////////////////////////////////////
        // ---------- argument getters ---------- //
        ////////////////////////////////////////////

        template <typename T>
        T arg(const uint8_t i) const;

        int32_t getArgAsInt32(const size_t i) const { return getPod<int32_t>(i); }
        int64_t getArgAsInt64(const size_t i) const { return getPod<int64_t>(i); }
        float getArgAsFloat(const size_t i) const { return getPod<float>(i); }
        double getArgAsDouble(const size_t i) const { return getPod<double>(i); }
        String getArgAsString(const size_t i) const { return String(argBeg(i)); }
        Blob getArgAsBlob(const size_t i) const { Blob b; b.assign(argBeg(i) + 4, argEnd(i)); return b; }
        bool getArgAsBool(const size_t i) const
        {
            if      (getTypeTag(i) == TYPE_TAG_TRUE)  return true;
            else if (getTypeTag(i) == TYPE_TAG_FALSE) return false;
            return false;
        }


        //////////////////////////////////////////////////
        // ---------- argument type checkers ---------- //
        //////////////////////////////////////////////////

        bool isBool(const size_t i) const { return getTypeTag(i) == TYPE_TAG_TRUE || getTypeTag(i) == TYPE_TAG_FALSE; }
        bool isInt32(const size_t i) const { return getTypeTag(i) == TYPE_TAG_INT32; }
        bool isInt64(const size_t i) const { return getTypeTag(i) == TYPE_TAG_INT64; }
        bool isFloat(const size_t i) const { return getTypeTag(i) == TYPE_TAG_FLOAT; }
        bool isDouble(const size_t i) const { return getTypeTag(i) == TYPE_TAG_DOUBLE; }
        bool isStr(const size_t i) const { return getTypeTag(i) == TYPE_TAG_STRING; }
        bool isBlob(const size_t i) const { return getTypeTag(i) == TYPE_TAG_BLOB; }

        const String& typeTags() const { return type_tags; }
        int getTypeTag(const size_t i) const { return type_tags[i]; }


        ///////////////////////////////////////////////////
        // ---------- osc message information ---------- //
        ///////////////////////////////////////////////////

        const String& address() const { return address_str; }
        size_t size() const { return type_tags.length(); }

        void remoteIP(const String& addr) { remote_ip = addr; }
        void remoteIP(const IPAddress& addr) { remote_ip = String(addr[0]) + "." + String(addr[1]) + "." + String(addr[2]) + "." + String(addr[3]); }
        void remoteIP(const char* addr) { remote_ip = String(addr); }
        void remotePort(const uint16_t p) { remote_port = p; }

        const String& remoteIP() const { return remote_ip; }
        uint16_t remotePort() const { return remote_port; }

        TimeTag timeTag() const { return time_tag; }


        ////////////////////////////////////////////////
        // ---------- osc message handling ---------- //
        ////////////////////////////////////////////////

    private:

        bool buildFromRawData(const void *ptr, const size_t sz)
        {
            clear();
            storage.assign((const char*)ptr, (const char*)ptr + sz);
            const char* const address_beg = storage.begin();
            const char* const address_end = (const char*)memchr(address_beg, 0, storage.end() - address_beg);

            if (!address_end || address_beg[0] != '/')
            {
                return false;
            }
            else
            {
                address_str = String("");
                const char* p = address_beg;
                while (p != address_end) { address_str += *p++; };
            }

            const char* const type_tags_beg = ceil4(address_end + 1 - address_beg) + address_beg;
            const char* type_tags_end = (const char*)memchr(type_tags_beg, 0, storage.end()-type_tags_beg);
            if (!type_tags_end || type_tags_beg[0] != ',')
            {
                return false;
            }
            else
            {
                type_tags = String("");
                const char* p = type_tags_beg + 1; // we do not copy the initial ','
                while (p != type_tags_end) type_tags += *p++;
            }

            const char* arg = ceil4(type_tags_end + 1 - address_beg) + address_beg;
            size_t iarg = 0;
            while (iarg < type_tags.length())
            {
                size_t len = getArgSize(type_tags[iarg], arg);
                arguments.push_back(make_pair((size_t)(arg - storage.begin()), len));
                arg += ceil4(len); ++iarg;
            }

            if (iarg < type_tags.length() || arg != storage.end())
            {
                return false;
            }

            return true;
        }

        const char* argBeg(const size_t idx) const
        {
            if (idx >= arguments.size()) return 0;
            return storage.begin() + arguments[idx].first;
        }

        const char *argEnd(const size_t idx) const
        {
            if (idx >= arguments.size()) return 0;
            return storage.begin() + arguments[idx].first + arguments[idx].second;
        }

        template <typename POD>
        POD getPod(const size_t idx) const
        {
            return bytes2pod<POD>(argBeg(idx));
        }

        size_t getArgSize(const int type, const char* const p) const
        {
            size_t sz = 0;
#ifndef ARDUINOOSC_DISABLE_STL
            assert(p >= storage.begin() && p <= storage.end());
#endif
            switch (type)
            {
                case TYPE_TAG_TRUE:
                case TYPE_TAG_FALSE:
                {
                    sz = 0;
                    break;
                }
                case TYPE_TAG_INT32:
                case TYPE_TAG_FLOAT:
                {
                    sz = 4;
                    break;
                }
                case TYPE_TAG_INT64:
                case TYPE_TAG_DOUBLE:
                {
                    sz = 8;
                    break;
                }
                case TYPE_TAG_STRING:
                {
                    const char* const q = (const char*)memchr(p, 0, storage.end() - p);
                    sz = (q - p) + 1;
                    break;
                }
                case TYPE_TAG_BLOB:
                {
                    if (p == storage.end()) return 0;
                    sz = 4 + bytes2pod<uint32_t>(p);
                    break;
                }
                default:
                {
                    return 0;
                    break;
                }
            }
            if ((p + sz > storage.end()) || // string or blob too large
                (p+sz < p)                  // or even blob so large that it did overflow
            )
                return 0;

            return sz;
        }

        template <typename POD>
        Message& pushPod(const int tag, const POD& v)
        {
            type_tags += (char)tag;
            arguments.push_back(make_pair(storage.size(), sizeof(POD)));
            pod2bytes(v, storage.getBytes(sizeof(POD)));
            return *this;
        }
    };


    template <> bool Message::arg<bool>(const uint8_t i) const { return getArgAsBool(i); }
    template <> char Message::arg<char>(const uint8_t i) const { return (char)getPod<int32_t>(i); }
    template <> signed char Message::arg<signed char>(const uint8_t i) const { return (signed char)getPod<int32_t>(i); }
    template <> unsigned char Message::arg<unsigned char>(const uint8_t i) const { return (unsigned char)getPod<int32_t>(i); }
    template <> short Message::arg<short>(const uint8_t i) const { return (short)getPod<int32_t>(i); }
    template <> unsigned short Message::arg<unsigned short>(const uint8_t i) const { return (unsigned short)getPod<int32_t>(i); }
    template <> int Message::arg<int>(const uint8_t i) const { return (int)getPod<int32_t>(i); }
    template <> unsigned Message::arg<unsigned>(const uint8_t i) const { return (unsigned)getPod<int32_t>(i); }
    template <> long Message::arg<long>(const uint8_t i) const { return (long)getPod<int32_t>(i); }
    template <> unsigned long Message::arg<unsigned long>(const uint8_t i) const { return (unsigned long)getPod<int32_t>(i); }
    template <> long long Message::arg<long long>(const uint8_t i) const { return (long long)getPod<int64_t>(i); }
    template <> unsigned long long Message::arg<unsigned long long>(const uint8_t i) const { return (unsigned long long)getPod<int64_t>(i); }
    template <> float Message::arg<float>(const uint8_t i) const { return getPod<float>(i); }
    template <> double Message::arg<double>(const uint8_t i) const { return getPod<double>(i); }
    template <> String Message::arg<String>(const uint8_t i) const { return getArgAsString(i); }
    template <> Blob Message::arg<Blob>(const uint8_t i) const { return getArgAsBlob(i); }

    template <> Message& Message::push<bool>(const bool& t) { return pushBool(t); }
    template <> Message& Message::push<char>(const char& t) { return pushInt32(t); }
    template <> Message& Message::push<signed char>(const signed char& t) { return pushInt32(t); }
    template <> Message& Message::push<unsigned char>(const unsigned char& t) { return pushInt32(t); }
    template <> Message& Message::push<short>(const short& t) { return pushInt32(t); }
    template <> Message& Message::push<unsigned short>(const unsigned short& t) { return pushInt32(t); }
    template <> Message& Message::push<int>(const int& t) { return pushInt32(t); }
    template <> Message& Message::push<unsigned>(const unsigned& t) { return pushInt32(t); }
    template <> Message& Message::push<long>(const long& t) { return pushInt32(t); }
    template <> Message& Message::push<unsigned long>(const unsigned long& t) { return pushInt32(t); }
    template <> Message& Message::push<long long>(const long long& t) { return pushInt64(t); }
    template <> Message& Message::push<unsigned long long>(const unsigned long long& t) { return pushInt64(t); }
    template <> Message& Message::push<float>(const float& t) { return pushFloat(t); }
    template <> Message& Message::push<double>(const double& t) { return pushDouble(t); }
    template <> Message& Message::push<String>(const String& t) { return pushString(t); }
    template <> Message& Message::push<Blob>(const Blob& t) { return pushBlob(t); }

} // namespace message
} // namespace osc
} // namespac arduino


using OscMessage = arduino::osc::message::Message;


#endif // ARDUINOOSC_OSCMESSAGE_H
