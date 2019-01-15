// 2018.12.31 Hideaki Tai
// This library is optimized to use mainly in Arduino
// and dropped support of following features
// - robust wrt malformed packets
// - optional udp transport for packets
// - error code handling

/** @mainpage OSCPKT : a minimalistic OSC ( http://opensoundcontrol.org ) c++ library

Before using this file please take the time to read the OSC spec, it
is short and not complicated: http://opensoundcontrol.org/spec-1_0

Features:
- handles basic OSC types: TFihfdsb
- handles bundles
- handles OSC pattern-matching rules (wildcards etc in message paths)
- portable on win / macos / linux
- robust wrt malformed packets
- optional udp transport for packets
- concise, all in a single .h file
- does not throw exceptions

does not:
- take into account timestamp values.
- provide a cpu-scalable message dispatching.
- not suitable for use inside a realtime thread as it allocates memory when
building or reading messages.


There are basically 3 classes of interest:
- oscpkt::Message       : read/write the content of an OSC message
- oscpkt::PacketReader  : read the bundles/messages embedded in an OSC packet
- oscpkt::PacketWriter  : write bundles/messages into an OSC packet

And optionaly:
- oscpkt::UdpSocket     : read/write OSC packets over UDP.

@example: oscpkt_demo.cc
@example: oscpkt_test.cc
*/

/* Copyright (C) 2010  Julien Pommier

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

(this is the zlib license)
*/

#ifndef OSCPKT_HH
#define OSCPKT_HH

#include <Arduino.h>

#ifndef _MSC_VER
#include <stdint.h>
#else
namespace oscpkt
{
    typedef __int32 int32_t;
    typedef unsigned __int32 uint32_t;
    typedef __int64 int64_t;
    typedef unsigned __int64 uint64_t;
}
#endif
#ifdef __AVR__
#include "RingBuffer.h"
namespace oscpkt
{
    struct ArgumentType { size_t first; size_t second; };
    using ArgumentQueue = RingBuffer<ArgumentType, 3>;
    using Blob = RingBuffer<char, 64>;
    class Message;
    using MessageQueue = RingBuffer<Message, 1>;
    using MessageIterator = Message*;
    using BundleType = RingBuffer<size_t, 0>; // disabled
}
namespace std
{
    oscpkt::ArgumentType make_pair(size_t first, size_t second)
    {
        return oscpkt::ArgumentType {first, second};
    }
}
#else
#include <cstring>
#include <cassert>
#include <string>
#include <vector>
#include <functional>
namespace oscpkt
{
    using ArgumentType = std::pair<size_t, size_t>;
    using ArgumentQueue = std::vector<ArgumentType>;
    using Blob = std::vector<char>;
    class Message;
    using MessageQueue = std::vector<Message>;
    using MessageIterator = std::vector<Message>::iterator;
    using BundleType = std::vector<size_t>;
}
#endif


namespace oscpkt
{
    class TimeTag
    {
        uint64_t v;
    public:
        TimeTag() : v(1) {}
        explicit TimeTag(uint64_t w): v(w) {}
        operator uint64_t() const { return v; }
        static TimeTag immediate() { return TimeTag(1); }
    };

    enum
    {
        TYPE_TAG_TRUE = 'T',
        TYPE_TAG_FALSE = 'F',
        TYPE_TAG_INT32 = 'i',
        TYPE_TAG_INT64 = 'h',
        TYPE_TAG_FLOAT = 'f',
        TYPE_TAG_DOUBLE = 'd',
        TYPE_TAG_STRING = 's',
        TYPE_TAG_BLOB = 'b'
    };

    // round to the next multiple of 4, works for size_t and pointer arguments
    template <typename Type>
    inline Type ceil4(Type p) { return (Type)((size_t(p) + 3) & (~size_t(3))); }

    inline bool isZeroPaddingCorrect(const char *p)
    {
        const char *q = ceil4(p);
        for (;p < q; ++p) if (*p != 0) return false;
        return true;
    }

    template <typename POD>
    union PodBytes
    {
        POD  value;
        char bytes[sizeof(POD)];
    };

    inline bool isBigEndian()
    {
        const PodBytes<int32_t> p {0x12345678};
        return (p.bytes[0] == 0x12);
    }

    template <typename POD>
    POD bytes2pod(const char *bytes)
    {
        PodBytes<POD> p;
        for (size_t i = 0; i < sizeof(POD); ++i)
        {
            if (isBigEndian()) p.bytes[i] = bytes[i];
            else               p.bytes[i] = bytes[sizeof(POD) - i - 1];
        }
        return p.value;
    }

    template <typename POD>
    void pod2bytes(const POD& value, char *bytes)
    {
        const PodBytes<POD> p {value};
        for (size_t i = 0; i < sizeof(POD); ++i)
        {
            if (isBigEndian()) bytes[i] = p.bytes[i];
            else               bytes[i] = p.bytes[sizeof(POD) - i - 1];
        }
    }


    struct Storage
    {
        Blob data;
        #ifndef __AVR__
        Storage() { data.reserve(200); }
        #endif
        char *getBytes(size_t sz)
        {
            #ifndef __AVR__
            assert((data.size() & 3) == 0);
            if (data.size() + sz > data.capacity()) { data.reserve((data.size() + sz) * 2); }
            #else
            if (data.size() + sz > data.capacity()) { return nullptr; }
            #endif
            size_t sz4 = ceil4(sz);
            size_t pos = data.size();
            data.resize(pos + sz4); // resize will fill with zeros, so the zero padding is OK
            return &(data[pos]);
        }
        char *begin() { return data.size() ? &data.front() : nullptr; }
        char *end() { return begin() ? (begin() + size()) : nullptr; }
        const char *begin() const { return data.size() ? &data.front() : nullptr; }
        const char *end() const { return begin() ? (begin() + size()) : nullptr; }
        size_t size() const { return data.size(); }
        void assign(const char *beg, const char *end) { data.assign(beg, end); }
        // #ifndef __AVR__
        void clear() { data.resize(0); }
        // #else
        // void clear() { for (size_t i = 0; i < data.capacity(); ++i) data.data()[i] = 0; }
        // #endif
    };

    bool fullPatternMatch(const String &pattern, const String &path);
    bool partialPatternMatch(const String &pattern, const String &path);


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

        String ip_addr;
        uint16_t udp_port;

    public:

        Message() { clear(); }
        Message(const String &s, TimeTag tt = TimeTag::immediate()) : time_tag(tt), address_str(s) { init(s, tt); }
        #ifndef ARDUINO_AVR_UNO // TODO: dirty...
        Message(const void *ptr, size_t sz, TimeTag tt = TimeTag::immediate()) { valid = buildFromRawData(ptr, sz); time_tag = tt; }
        #else
        Message(const void *ptr, size_t sz, TimeTag tt = TimeTag::immediate()) { buildFromRawData(ptr, sz); time_tag = tt; }
        #endif
        Message(const String& ip, uint16_t port, const String& addr) : ip_addr(ip) , udp_port(port) { init(addr); }

        virtual ~Message() {}

        inline bool available()
        {
            #ifndef ARDUINO_AVR_UNO // TODO: dirty...
            return valid;
            #else
            return true;
            #endif
        }

        inline bool isBool(size_t i) const { return getTypeTag(i) == TYPE_TAG_TRUE || getTypeTag(i) == TYPE_TAG_FALSE; }
        inline bool isInt32(size_t i) const { return getTypeTag(i) == TYPE_TAG_INT32; }
        inline bool isInt64(size_t i) const { return getTypeTag(i) == TYPE_TAG_INT64; }
        inline bool isFloat(size_t i) const { return getTypeTag(i) == TYPE_TAG_FLOAT; }
        inline bool isDouble(size_t i) const { return getTypeTag(i) == TYPE_TAG_DOUBLE; }
        inline bool isStr(size_t i) const { return getTypeTag(i) == TYPE_TAG_STRING; }
        inline bool isBlob(size_t i) const { return getTypeTag(i) == TYPE_TAG_BLOB; }

        inline int32_t getArgAsInt32(size_t i) { return getPod<int32_t>(i); }
        inline int64_t getArgAsInt64(size_t i) { return getPod<int64_t>(i); }
        inline float getArgAsFloat(size_t i) { return getPod<float>(i); }
        inline double getArgAsDouble(size_t i) { return getPod<double>(i); }
        inline String getArgAsString(size_t i) { return String(argBeg(i)); }
        inline Blob getArgAsBlob(size_t i) { Blob b; b.assign(argBeg(i) + 4, argEnd(i)); return b; }
        inline const uint8_t* getArgAsBlob(size_t i) const { return reinterpret_cast<const uint8_t*>(argBeg(i) + 4); }
        inline bool getArgAsBool(size_t i)
        {
            if      (getTypeTag(i) == TYPE_TAG_TRUE)  return true;
            else if (getTypeTag(i) == TYPE_TAG_FALSE) return false;
            return false;
        }

        #ifndef __AVR__
        template <typename T> inline T arg(uint8_t i); // TODO: make smarter to get arguments
        #endif

        const String &typeTags() const { return type_tags; }
        int getTypeTag(size_t i) const { return type_tags[i]; }

        const String &address() const { return address_str; }
        size_t size() { return type_tags.length(); }

        TimeTag timeTag() const { return time_tag; }
        Message &init(const String &addr, TimeTag tt = TimeTag::immediate())
        {
            clear(); address_str = addr; time_tag = tt;
            return *this;
        }

        bool match(const String &pattern, bool full = true) const
        {
            if (full) return fullPatternMatch(pattern.c_str(), address_str.c_str());
            else      return partialPatternMatch(pattern.c_str(), address_str.c_str());
        }

        inline Message &pushBool(bool b)
        {
            type_tags += (char)(b ? TYPE_TAG_TRUE : TYPE_TAG_FALSE);
            arguments.push_back(std::make_pair(storage.size(), storage.size()));
            return *this;
        }
        inline Message &pushInt32(int32_t i) { return pushPod(TYPE_TAG_INT32, i); }
        inline Message &pushInt64(int64_t h) { return pushPod(TYPE_TAG_INT64, h); }
        inline Message &pushFloat(float f) { return pushPod(TYPE_TAG_FLOAT, f); }
        inline Message &pushDouble(double d) { return pushPod(TYPE_TAG_DOUBLE, d); }
        inline Message &pushString(const String &s)
        {
            type_tags += (char)TYPE_TAG_STRING;
            arguments.push_back(std::make_pair(storage.size(), s.length() + 1));
            strcpy(storage.getBytes(s.length() + 1), s.c_str());
            return *this;
        }
        inline Message &pushBlob(const void *ptr, size_t num_bytes)
        {
            type_tags += (char)TYPE_TAG_BLOB;
            arguments.push_back(std::make_pair(storage.size(), num_bytes + 4));
            pod2bytes<int32_t>((int32_t)num_bytes, storage.getBytes(4));
            if (num_bytes) memcpy(storage.getBytes(num_bytes), ptr, num_bytes);
            return *this;
        }
        inline Message &pushBlob(const Blob& b) { return pushBlob(b.data(), b.size()); }

        // TODO: reference, move....
        // template <typename T> inline Message& push(const T& t);
        // template <typename T> inline Message& push(T&& t);
        template <typename T> inline Message& push(T t);
        inline Message& push(const char* c) { return pushString(String(c)); }

        void clear()
        {
            address_str = type_tags = String("");
            storage.clear();
            arguments.clear();
            time_tag = TimeTag::immediate();
        }

        void encode(Storage &s, bool write_size = false) const
        {
            size_t l_addr = address_str.length() + 1;
            size_t l_type = type_tags.length() + 2;
            if (write_size)
                pod2bytes<uint32_t>(uint32_t(ceil4(l_addr) + ceil4(l_type) + ceil4(storage.size())), s.getBytes(4));
            strcpy(s.getBytes(l_addr), address_str.c_str());
            strcpy(s.getBytes(l_type), ("," + type_tags).c_str());
            if (storage.size())
                memcpy(s.getBytes(storage.size()), const_cast<Storage&>(storage).begin(), storage.size());
        }

        void ip(const String& addr) { ip_addr = addr; }
        void ip(const IPAddress& addr) { ip_addr = String(addr[0]) + "." + String(addr[1]) + "." + String(addr[2]) + "." + String(addr[3]); }
        void ip(const char* addr) { ip_addr = String(addr); }

        void port(uint16_t p) { udp_port = p; }

        const String& ip() const { return ip_addr; }
        // IPAddress ip() const { IPAddress addr; addr.fromString(ip_addr); return addr; }

        uint16_t port() const { return udp_port; }

    private:

        bool buildFromRawData(const void *ptr, size_t sz)
        {
            clear();
            storage.assign((const char*)ptr, (const char*)ptr + sz);
            const char* const address_beg = storage.begin();
            const char* const address_end = (const char*)memchr(address_beg, 0, storage.end() - address_beg);

            // TODO: fix bug for AVR boards in which memory layout is not correct.
            // if bool dummy; is added in this class, building from raw data fails because of ceil4 return not correct pointer address
            if (!address_end || !isZeroPaddingCorrect(address_end + 1) || address_beg[0] != '/')
            {
                return false;
            }
            else
            {
                address_str = String("");
                const char* p = address_beg;
                while (p != address_end) { address_str += *p++; };
            }

            const char* const type_tags_beg = ceil4(address_end + 1);
            const char *type_tags_end = (const char*)memchr(type_tags_beg, 0, storage.end()-type_tags_beg);
            if (!type_tags_end || !isZeroPaddingCorrect(type_tags_end + 1) || type_tags_beg[0] != ',')
            {
                return false;
            }
            else
            {
                type_tags = String("");
                const char* p = type_tags_beg + 1; // we do not copy the initial ','
                while (p != type_tags_end) type_tags += *p++;
            }

            const char *arg = ceil4(type_tags_end+1);
            size_t iarg = 0;
            while (iarg < type_tags.length())
            {
                size_t len = getArgSize(type_tags[iarg], arg);
                arguments.push_back(std::make_pair(arg - storage.begin(), len));
                arg += ceil4(len); ++iarg;
            }

            if (iarg < type_tags.length() || arg != storage.end())
            {
                return false;
            }

            return true;
        }

        const char *argBeg(size_t idx) const
        {
            if (idx >= arguments.size()) return 0;
            return storage.begin() + arguments[idx].first;
        }
        const char *argEnd(size_t idx) const
        {
            if (idx >= arguments.size()) return 0;
            return storage.begin() + arguments[idx].first + arguments[idx].second;
        }
        template <typename POD>
        POD getPod(size_t idx)
        {
            return bytes2pod<POD>(argBeg(idx));
        }

        size_t getArgSize(int type, const char* const p)
        {
            // if (err) return 0;
            size_t sz = 0;
            #ifndef __AVR__
            assert(p >= storage.begin() && p <= storage.end());
            #endif
            switch (type)
            {
                case TYPE_TAG_TRUE:
                case TYPE_TAG_FALSE: sz = 0; break;
                case TYPE_TAG_INT32:
                case TYPE_TAG_FLOAT: sz = 4; break;
                case TYPE_TAG_INT64:
                case TYPE_TAG_DOUBLE: sz = 8; break;
                case TYPE_TAG_STRING:
                {
                    const char* const q = (const char*)memchr(p, 0, storage.end()-p);
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
            if (p+sz > storage.end() || /* string or blob too large.. */
            p+sz < p /* or even blob so large that it did overflow */)
                return 0;
            if (!isZeroPaddingCorrect(p+sz))
                return 0;
            return sz;
        }

        template <typename POD>
        Message &pushPod(int tag, const POD& v)
        {
            type_tags += (char)tag;
            arguments.push_back(std::make_pair(storage.size(), sizeof(POD)));
            pod2bytes(v, storage.getBytes(sizeof(POD)));
            return *this;
        }
    };

    #ifndef __AVR__
    #ifdef TEENSYDUINO
    template <> inline int Message::arg<int>(uint8_t i) { return getPod<int>(i); }
    #endif
    template <> inline bool Message::arg<bool>(uint8_t i) { return getArgAsBool(i); }
    template <> inline int32_t Message::arg<int32_t>(uint8_t i) { return getPod<int32_t>(i); }
    template <> inline int64_t Message::arg<int64_t>(uint8_t i) { return getPod<int64_t>(i); }
    template <> inline float Message::arg<float>(uint8_t i) { return getPod<float>(i); }
    template <> inline double Message::arg<double>(uint8_t i) { return getPod<double>(i); }
    template <> inline String Message::arg<String>(uint8_t i) { return getArgAsString(i); }
    template <> inline Blob Message::arg<Blob>(uint8_t i) { return getArgAsBlob(i); }
    #endif

    #ifdef TEENSYDUINO
    // template <> inline Message& Message::push<int>(const int& t) { return pushInt32(t); }
    // template <> inline Message& Message::push<int>(int&& t) { return pushInt32(t); }
    template <> inline Message& Message::push<int>(int t) { return pushInt32(t); }
    #endif
    // template <> inline Message& Message::push<bool>(const bool& t) { return pushBool(t); }
    // template <> inline Message& Message::push<uint8_t>(const uint8_t& t) { return pushInt32(t); }
    // template <> inline Message& Message::push<uint16_t>(const uint16_t& t) { return pushInt32(t); }
    // template <> inline Message& Message::push<uint32_t>(const uint32_t& t) { return pushInt32(t); }
    // template <> inline Message& Message::push<uint64_t>(const uint64_t& t) { return pushInt64(t); }
    // template <> inline Message& Message::push<int8_t>(const int8_t& t) { return pushInt32(t); }
    // template <> inline Message& Message::push<int16_t>(const int16_t& t) { return pushInt32(t); }
    // template <> inline Message& Message::push<int32_t>(const int32_t& t) { return pushInt32(t); }
    // template <> inline Message& Message::push<int64_t>(const int64_t& t) { return pushInt64(t); }
    // template <> inline Message& Message::push<float>(const float& t) { return pushFloat(t); }
    // template <> inline Message& Message::push<double>(const double& t) { return pushDouble(t); }
    // template <> inline Message& Message::push<String>(const String& t) { return pushString(t); }
    // template <> inline Message& Message::push<Blob>(const Blob& t) { return pushBlob(t); }

    // template <> inline Message& Message::push<bool>(bool&& t) { return pushBool(t); }
    // template <> inline Message& Message::push<uint8_t>(uint8_t&& t) { return pushInt32(t); }
    // template <> inline Message& Message::push<uint16_t>(uint16_t&& t) { return pushInt32(t); }
    // template <> inline Message& Message::push<uint32_t>(uint32_t&& t) { return pushInt32(t); }
    // template <> inline Message& Message::push<uint64_t>(uint64_t&& t) { return pushInt64(t); }
    // template <> inline Message& Message::push<int8_t>(int8_t&& t) { return pushInt32(t); }
    // template <> inline Message& Message::push<int16_t>(int16_t&& t) { return pushInt32(t); }
    // template <> inline Message& Message::push<int32_t>(int32_t&& t) { return pushInt32(t); }
    // template <> inline Message& Message::push<int64_t>(int64_t&& t) { return pushInt64(t); }
    // template <> inline Message& Message::push<float>(float&& t) { return pushFloat(t); }
    // template <> inline Message& Message::push<double>(double&& t) { return pushDouble(t); }
    // template <> inline Message& Message::push<String>(String&& t) { return pushString(t); }
    // template <> inline Message& Message::push<Blob>(Blob&& t) { return pushBlob(t); }

    template <> inline Message& Message::push<bool>(bool t) { return pushBool(t); }
    template <> inline Message& Message::push<uint8_t>(uint8_t t) { return pushInt32(t); }
    template <> inline Message& Message::push<uint16_t>(uint16_t t) { return pushInt32(t); }
    template <> inline Message& Message::push<uint32_t>(uint32_t t) { return pushInt32(t); }
    template <> inline Message& Message::push<uint64_t>(uint64_t t) { return pushInt64(t); }
    template <> inline Message& Message::push<int8_t>(int8_t t) { return pushInt32(t); }
    template <> inline Message& Message::push<int16_t>(int16_t t) { return pushInt32(t); }
    template <> inline Message& Message::push<int32_t>(int32_t t) { return pushInt32(t); }
    template <> inline Message& Message::push<int64_t>(int64_t t) { return pushInt64(t); }
    template <> inline Message& Message::push<float>(float t) { return pushFloat(t); }
    template <> inline Message& Message::push<double>(double t) { return pushDouble(t); }
    template <> inline Message& Message::push<String>(String t) { return pushString(t); }
    template <> inline Message& Message::push<Blob>(Blob t) { return pushBlob(t); }

    class PacketReader
    {
    public:

        PacketReader() {}
        PacketReader(const void *ptr, size_t sz) { init(ptr, sz); }

        bool init(const void *ptr, size_t sz)
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
            if (!messages.empty() && it_messages != messages.end())
                return &*it_messages++;
            return nullptr;
        }

    private:

        bool parse(const char *beg, const char *end, TimeTag time_tag)
        {
            if (beg == end) return false;
            if (*beg == '#')
            {
                if (end - beg >= 20 && memcmp(beg, "#bundle\0", 8) == 0)
                {
                    TimeTag time_tag2(bytes2pod<uint64_t>(beg + 8));
                    const char *pos = beg + 16;
                    do {
                        uint32_t sz = bytes2pod<uint32_t>(pos);
                        pos += 4;
                        if ((sz & 3) != 0 || pos + sz > end || pos+sz < pos)
                            return false;
                        else
                        {
                            parse(pos, pos + sz, time_tag2);
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

        MessageQueue messages;
        MessageIterator it_messages;
    };


    class PacketWriter
    {
    public:

        PacketWriter() { init(); }
        PacketWriter &init()
        {
            storage.clear();
            bundles.clear();
            return *this;
        }

        PacketWriter& encode(const Message &msg)
        {
            msg.encode(storage, !bundles.empty());
            return *this;
        }

        uint32_t size() { return (uint32_t)storage.size(); }
        char* data() { return storage.begin(); }

        PacketWriter &begin_bundle(TimeTag ts = TimeTag::immediate())
        {
            char *p;
            if (bundles.size()) p = storage.getBytes(4); // hold the bundle size
            p = storage.getBytes(8);
            strcpy(p, "#bundle");
            bundles.push_back(p - storage.begin());
            p = storage.getBytes(8);
            pod2bytes<uint64_t>(ts, p);
            return *this;
        }
        PacketWriter &end_bundle()
        {
            if (bundles.size())
            {
                if (storage.size() - bundles.back() == 16)
                    pod2bytes<uint32_t>(0, storage.getBytes(4)); // the 'empty bundle' case, not very elegant
                if (bundles.size() > 1)
                    pod2bytes<uint32_t>(uint32_t(storage.size() - bundles.back()), storage.begin() + bundles.back()-4);
                bundles.pop_back();
            }
            return *this;
        }

    private:

        Storage storage;
        BundleType bundles;
    };


    inline const char *internalPatternMatch(const char *pattern, const char *path)
    {
        while (*pattern)
        {
            const char *p = pattern;
            if ((*p == '?') && *path) { ++p; ++path; }
            else if ((*p == '[') && *path)
            { // bracketted range, e.g. [a-zABC]
                ++p;
                bool reverse = false;
                if (*p == '!') { reverse = true; ++p; }
                bool match = reverse;
                for (; *p && (*p != ']'); ++p)
                {
                    char c0 = *p, c1 = c0;
                    if ((p[1] == '-') && p[2]) { p += 2; c1 = *p; }
                    if ((*path >= c0) && (*path <= c1)) { match = !reverse; }
                }
                if (!match || (*p != ']')) return pattern;
                ++p; ++path;
            }
            else if (*p == '*')
            { // wildcard '*'
                while (*p == '*') ++p;
                const char *best = 0;
                while (true)
                {
                    const char *ret = internalPatternMatch(p, path);
                    if (ret && (ret > best)) best = ret;
                    if ((*path == 0) || (*path == '/')) break;
                    else ++path;
                }
                return best;
            }
            else if ((*p == '/') && (*(p + 1) == '/'))
            { // the super-wildcard '//'
                while (*(p + 1) == '/') ++p;
                const char *best = 0;
                while (true)
                {
                    const char *ret = internalPatternMatch(p, path);
                    if (ret && (ret > best)) best = ret;
                    if (*path == 0) break;
                    if ((*path == 0) || ((path = strchr(path+1, '/')) == 0)) break;
                }
                return best;
            }
            else if (*p == '{')
            { // braced list {foo,bar,baz}
                const char *end = strchr(p, '}'), *q;
                if (!end) return 0; // syntax error in brace list..
                bool match = false;
                do {
                    ++p;
                    q = strchr(p, ',');
                    if ((q == 0) || (q > end)) q = end;
                    if (strncmp(p, path, q - p) == 0)
                    {
                        path += (q - p);
                        p = end + 1;
                        match = true;
                    }
                    else p = q;
                }
                while ((q != end) && !match);
                if (!match) return pattern;
            }
            else if (*p == *path) { ++p; ++path; } // any other character
            else break;
            pattern = p;
        }
        return (*path == 0 ? pattern : 0);
    }

    inline bool partialPatternMatch(const String &pattern, const String &test)
    {
        const char *q = internalPatternMatch(pattern.c_str(), test.c_str());
        return q != 0;
    }

    inline bool fullPatternMatch(const String &pattern, const String &test)
    {
        const char *q = internalPatternMatch(pattern.c_str(), test.c_str());
        return q && *q == 0;
    }

} // namespace oscpkt

#endif // OSCPKT_HH
