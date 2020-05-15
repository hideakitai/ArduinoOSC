#pragma once

#ifndef ARDUINOOSC_OSCTYPES_H
#define ARDUINOOSC_OSCTYPES_H

#include <Arduino.h>

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

#include "OscUtil.h"

namespace oscpkt
{
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

    class TimeTag
    {
        uint64_t v;
    public:
        TimeTag() : v(1) {}
        explicit TimeTag(uint64_t w): v(w) {}
        operator uint64_t() const { return v; }
        static TimeTag immediate() { return TimeTag(1); }
    };

    struct Storage
    {
        Blob data;
#ifndef __AVR__
        Storage() { data.reserve(200); }
#endif
        char* getBytes(size_t sz)
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
        char* begin() { return data.size() ? &data.front() : nullptr; }
        char* end() { return begin() ? (begin() + size()) : nullptr; }
        const char* begin() const { return data.size() ? &data.front() : nullptr; }
        const char* end() const { return begin() ? (begin() + size()) : nullptr; }
        size_t size() const { return data.size(); }
        void assign(const char *beg, const char *end) { data.assign(beg, end); }
        void clear() { data.resize(0); }
    };

} // namespace oscpkt

#endif // ARDUINOOSC_OSCTYPES_H
