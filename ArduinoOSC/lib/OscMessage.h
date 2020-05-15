#pragma once

#ifndef ARDUINOOSC_OSCMESSAGE_H
#define ARDUINOOSC_OSCMESSAGE_H

namespace oscpkt {

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

        Message()
        {
            clear();
        }
        Message(const String &s, TimeTag tt = TimeTag::immediate())
        : time_tag(tt), address_str(s)
        {
            init(s, tt);
        }
#ifndef ARDUINO_AVR_UNO // TODO: dirty...
        Message(const void *ptr, size_t sz, TimeTag tt = TimeTag::immediate())
        {
            valid = buildFromRawData(ptr, sz);
            time_tag = tt;
        }
#else
        Message(const void *ptr, size_t sz, TimeTag tt = TimeTag::immediate())
        {
            buildFromRawData(ptr, sz);
            time_tag = tt;
        }
#endif
        Message(const String& ip, uint16_t port, const String& addr)
        : ip_addr(ip) , udp_port(port)
        {
            init(addr);
        }

        virtual ~Message() {}

        inline bool available()
        {
#ifndef ARDUINO_AVR_UNO // TODO: dirty...
            return valid;
#else
            return true;
#endif
        }


        ////////////////////////////////////////////
        // ---------- adding arguments ---------- //
        ////////////////////////////////////////////

        // TODO: reference, move....
        // template <typename T> inline Message& push(const T& t);
        // template <typename T> inline Message& push(T&& t);
        template <typename T>
        inline Message& push(T t);
        inline Message& push(const char* c) { return pushString(String(c)); }

        inline Message& pushBool(bool b)
        {
            type_tags += (char)(b ? TYPE_TAG_TRUE : TYPE_TAG_FALSE);
            arguments.push_back(std::make_pair(storage.size(), storage.size()));
            return *this;
        }
        inline Message& pushInt32(const int32_t i) { return pushPod(TYPE_TAG_INT32, i); }
        inline Message& pushInt64(const int64_t h) { return pushPod(TYPE_TAG_INT64, h); }
        inline Message& pushFloat(const float f) { return pushPod(TYPE_TAG_FLOAT, f); }
        inline Message& pushDouble(const double d) { return pushPod(TYPE_TAG_DOUBLE, d); }
        inline Message& pushString(const String &s)
        {
            type_tags += (char)TYPE_TAG_STRING;
            arguments.push_back(std::make_pair(storage.size(), s.length() + 1));
            strcpy(storage.getBytes(s.length() + 1), s.c_str());
            return *this;
        }
        inline Message& pushBlob(const void *ptr, const size_t num_bytes)
        {
            type_tags += (char)TYPE_TAG_BLOB;
            arguments.push_back(std::make_pair(storage.size(), num_bytes + 4));
            pod2bytes<int32_t>((int32_t)num_bytes, storage.getBytes(4));
            if (num_bytes) memcpy(storage.getBytes(num_bytes), ptr, num_bytes);
            return *this;
        }
        inline Message& pushBlob(const Blob& b) { return pushBlob(b.data(), b.size()); }

        void encode(Storage &s, bool write_size = false) const
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


        ////////////////////////////////////////////
        // ---------- argument getters ---------- //
        ////////////////////////////////////////////

#ifndef __AVR__
        template <typename T>
        inline T arg(uint8_t i);
#endif

        inline int32_t getArgAsInt32(size_t i) const { return getPod<int32_t>(i); }
        inline int64_t getArgAsInt64(size_t i) const { return getPod<int64_t>(i); }
        inline float getArgAsFloat(size_t i) const { return getPod<float>(i); }
        inline double getArgAsDouble(size_t i) const { return getPod<double>(i); }
        inline String getArgAsString(size_t i) const { return String(argBeg(i)); }
        // TODO: const
        inline Blob getArgAsBlob(size_t i) { Blob b; b.assign(argBeg(i) + 4, argEnd(i)); return b; }
        inline const uint8_t* getArgAsBlob(size_t i) const { return reinterpret_cast<const uint8_t*>(argBeg(i) + 4); }
        inline bool getArgAsBool(size_t i) const
        {
            if      (getTypeTag(i) == TYPE_TAG_TRUE)  return true;
            else if (getTypeTag(i) == TYPE_TAG_FALSE) return false;
            return false;
        }


        //////////////////////////////////////////////////
        // ---------- argument type checkers ---------- //
        //////////////////////////////////////////////////

        inline bool isBool(const size_t i) const { return getTypeTag(i) == TYPE_TAG_TRUE || getTypeTag(i) == TYPE_TAG_FALSE; }
        inline bool isInt32(const size_t i) const { return getTypeTag(i) == TYPE_TAG_INT32; }
        inline bool isInt64(const size_t i) const { return getTypeTag(i) == TYPE_TAG_INT64; }
        inline bool isFloat(const size_t i) const { return getTypeTag(i) == TYPE_TAG_FLOAT; }
        inline bool isDouble(const size_t i) const { return getTypeTag(i) == TYPE_TAG_DOUBLE; }
        inline bool isStr(const size_t i) const { return getTypeTag(i) == TYPE_TAG_STRING; }
        inline bool isBlob(const size_t i) const { return getTypeTag(i) == TYPE_TAG_BLOB; }

        const String& typeTags() const { return type_tags; }
        int getTypeTag(const size_t i) const { return type_tags[i]; }


        ///////////////////////////////////////////////////
        // ---------- osc message information ---------- //
        ///////////////////////////////////////////////////

        const String& address() const { return address_str; }
        size_t size() const { return type_tags.length(); }

        void ip(const String& addr) { ip_addr = addr; }
        void ip(const IPAddress& addr) { ip_addr = String(addr[0]) + "." + String(addr[1]) + "." + String(addr[2]) + "." + String(addr[3]); }
        void ip(const char* addr) { ip_addr = String(addr); }
        void port(const uint16_t p) { udp_port = p; }

        const String& ip() const { return ip_addr; }
        uint16_t port() const { return udp_port; }

        TimeTag timeTag() const { return time_tag; }


        ////////////////////////////////////////////////
        // ---------- osc message handling ---------- //
        ////////////////////////////////////////////////

        Message& init(const String& addr, TimeTag tt = TimeTag::immediate())
        {
            clear();
            address_str = addr;
            time_tag = tt;
            return *this;
        }

        bool match(const String& pattern, bool full = true) const
        {
            if (full)
                return fullPatternMatch(pattern.c_str(), address_str.c_str());
            else
                return partialPatternMatch(pattern.c_str(), address_str.c_str());
        }

        void clear()
        {
            address_str = type_tags = String("");
            storage.clear();
            arguments.clear();
            time_tag = TimeTag::immediate();
        }


    private:

        bool buildFromRawData(const void *ptr, const size_t sz)
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
            const char* type_tags_end = (const char*)memchr(type_tags_beg, 0, storage.end()-type_tags_beg);
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

            const char* arg = ceil4(type_tags_end+1);
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

        size_t getArgSize(const int type, const char* const p)
        {
            // if (err) return 0;
            size_t sz = 0;
#ifndef __AVR__
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

            if (!isZeroPaddingCorrect(p + sz))
                return 0;

            return sz;
        }

        template <typename POD>
        Message& pushPod(const int tag, const POD& v)
        {
            type_tags += (char)tag;
            arguments.push_back(std::make_pair(storage.size(), sizeof(POD)));
            pod2bytes(v, storage.getBytes(sizeof(POD)));
            return *this;
        }
    };


    template <> inline bool Message::arg<bool>(uint8_t i) { return getArgAsBool(i); }
    template <> inline char Message::arg<char>(uint8_t i) { return (char)getPod<int32_t>(i); }
    template <> inline signed char Message::arg<signed char>(uint8_t i) { return (signed char)getPod<int32_t>(i); }
    template <> inline unsigned char Message::arg<unsigned char>(uint8_t i) { return (unsigned char)getPod<int32_t>(i); }
    template <> inline short Message::arg<short>(uint8_t i) { return (short)getPod<int32_t>(i); }
    template <> inline unsigned short Message::arg<unsigned short>(uint8_t i) { return (unsigned short)getPod<int32_t>(i); }
    template <> inline int Message::arg<int>(uint8_t i) { return (int)getPod<int32_t>(i); }
    template <> inline unsigned Message::arg<unsigned>(uint8_t i) { return (unsigned)getPod<int32_t>(i); }
    template <> inline long Message::arg<long>(uint8_t i) { return (long)getPod<int32_t>(i); }
    template <> inline unsigned long Message::arg<unsigned long>(uint8_t i) { return (unsigned long)getPod<int32_t>(i); }
    template <> inline long long Message::arg<long long>(uint8_t i) { return (long long)getPod<int64_t>(i); }
    template <> inline unsigned long long Message::arg<unsigned long long>(uint8_t i) { return (unsigned long long)getPod<int64_t>(i); }
    template <> inline float Message::arg<float>(uint8_t i) { return getPod<float>(i); }
    template <> inline double Message::arg<double>(uint8_t i) { return getPod<double>(i); }
    template <> inline String Message::arg<String>(uint8_t i) { return getArgAsString(i); }
    template <> inline Blob Message::arg<Blob>(uint8_t i) { return getArgAsBlob(i); }

    template <> inline Message& Message::push<bool>(bool t) { return pushBool(t); }
    template <> inline Message& Message::push<char>(char t) { return pushInt32(t); }
    template <> inline Message& Message::push<signed char>(signed char t) { return pushInt32(t); }
    template <> inline Message& Message::push<unsigned char>(unsigned char t) { return pushInt32(t); }
    template <> inline Message& Message::push<short>(short t) { return pushInt32(t); }
    template <> inline Message& Message::push<unsigned short>(unsigned short t) { return pushInt32(t); }
    template <> inline Message& Message::push<int>(int t) { return pushInt32(t); }
    template <> inline Message& Message::push<unsigned>(unsigned t) { return pushInt32(t); }
    template <> inline Message& Message::push<long>(long t) { return pushInt32(t); }
    template <> inline Message& Message::push<unsigned long>(unsigned long t) { return pushInt32(t); }
    template <> inline Message& Message::push<long long>(long long t) { return pushInt64(t); }
    template <> inline Message& Message::push<unsigned long long>(unsigned long long t) { return pushInt64(t); }
    template <> inline Message& Message::push<float>(float t) { return pushFloat(t); }
    template <> inline Message& Message::push<double>(double t) { return pushDouble(t); }
    template <> inline Message& Message::push<String>(String t) { return pushString(t); }
    template <> inline Message& Message::push<Blob>(Blob t) { return pushBlob(t); }

} // oscpkt

#endif // ARDUINOOSC_OSCMESSAGE_H
