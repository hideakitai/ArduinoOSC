#pragma once

#ifndef ARDUINOOSC_OSCTYPES_H
#define ARDUINOOSC_OSCTYPES_H

#include <Arduino.h>
#include "util/ArxTypeTraits/ArxTypeTraits.h"

#ifdef ARDUINOOSC_DISABLE_STL

    #include "util/ArxContainer/ArxContainer.h"

    namespace arduino {
    namespace osc {

        #ifndef ARDUINOOSC_MAX_ARGUMENT_SIZE
        #define ARDUINOOSC_MAX_ARGUMENT_SIZE 8
        #endif
        #ifndef ARDUINOOSC_MAX_BLOB_BYTE_SIZE
        #define ARDUINOOSC_MAX_BLOB_BYTE_SIZE 64
        #endif
        #ifndef ARDUINOOSC_MAX_MSG_QUEUE_SIZE
        #define ARDUINOOSC_MAX_MSG_QUEUE_SIZE 1
        #endif
        #ifndef ARDUINOOSC_MAX_PUBLISH_DESTINATION
        #define ARDUINOOSC_MAX_PUBLISH_DESTINATION 4
        #endif
        #ifndef ARDUINOOSC_MAX_SUBSCRIBE_ADDRESS_PER_PORT
        #define ARDUINOOSC_MAX_SUBSCRIBE_ADDRESS_PER_PORT 4
        #endif
        #ifndef ARDUINOOSC_MAX_SUBSCRIBE_PORTS
        #define ARDUINOOSC_MAX_SUBSCRIBE_PORTS 2
        #endif
        #ifndef ARDUINOOSC_MAX_MSG_BUNDLE_SIZE
        #define ARDUINOOSC_MAX_MSG_BUNDLE_SIZE 32
        #endif

        namespace message
        {
            using ArgumentType = arx::pair<size_t, size_t>;
            using ArgumentQueue = arx::vector<ArgumentType, ARDUINOOSC_MAX_ARGUMENT_SIZE>;
            class Message;
            using MessageQueue = arx::vector<Message, ARDUINOOSC_MAX_MSG_QUEUE_SIZE>;
            using MessageIterator = Message*;
        }
#ifndef ARDUINOOSC_DISABLE_BUNDLE
        using BundleData = arx::vector<uint32_t, ARDUINOOSC_MAX_MSG_BUNDLE_SIZE>;
#endif
        using Blob = arx::vector<char, ARDUINOOSC_MAX_BLOB_BYTE_SIZE>;

        namespace client
        {
            namespace element
            {
                class Base;
                using Ref = arx::shared_ptr<Base>;
                using TupleRef = arx::vector<Ref, ARDUINOOSC_MAX_ARGUMENT_SIZE>;
            }
            class Destination;
            using ElementRef = element::Ref;
            using ElementTupleRef = element::TupleRef;
            using AddressMap = arx::map<Destination, ElementRef, ARDUINOOSC_MAX_PUBLISH_DESTINATION>;
        }

        namespace server
        {
            namespace element
            {
                class Base;
                using Ref = arx::shared_ptr<Base>;
                using TupleRef = arx::vector<Ref, ARDUINOOSC_MAX_ARGUMENT_SIZE>;
                using dummy_vector_t = arx::vector<size_t, ARDUINOOSC_MAX_ARGUMENT_SIZE>;
            }
            using ElementRef = element::Ref;
            using ElementTupleRef = element::TupleRef;
            using CallbackMap = arx::map<String, ElementRef, ARDUINOOSC_MAX_SUBSCRIBE_ADDRESS_PER_PORT>;
            template <typename S> class Server;
            template <typename S> using ServerRef = arx::shared_ptr<Server<S>>;
            template <typename S> using ServerMap = arx::map<uint16_t, ServerRef<S>, ARDUINOOSC_MAX_SUBSCRIBE_PORTS>;
        }

    } // namespace osc
    } // namespac arduino

#else

    #include <vector>
    #include <map>

    namespace arduino {
    namespace osc {

        namespace message
        {
            using ArgumentType = std::pair<size_t, size_t>;
            using ArgumentQueue = std::vector<ArgumentType>;
            class Message;
            using MessageQueue = std::vector<Message>;
            using MessageIterator = std::vector<Message>::iterator;
        }
        using BundleData = std::vector<uint32_t>;
        using Blob = std::vector<char>;

        namespace client
        {
            namespace element
            {
                class Base;
                using Ref = std::shared_ptr<Base>;
                using TupleRef = std::vector<Ref>;
            }
            class Destination;
            using ElementRef = element::Ref;
            using ElementTupleRef = element::TupleRef;
            using AddressMap = std::map<Destination, ElementRef>;
        }

        namespace server
        {
            namespace element
            {
                class Base;
                using Ref = std::shared_ptr<Base>;
                using TupleRef = std::vector<Ref>;
                using dummy_vector_t = std::vector<size_t>;
            }
            using ElementRef = element::Ref;
            using ElementTupleRef = element::TupleRef;
            using CallbackMap = std::map<String, ElementRef>;
            template <typename S> class Server;
            template <typename S> using ServerRef = std::shared_ptr<Server<S>>;
            template <typename S> using ServerMap = std::map<uint16_t, ServerRef<S>>;
        }

    } // namespace osc
    } // namespac arduino

#endif // ARDUINOOSC_DISABLE_STL


#include "OscUtil.h"

namespace arduino {
namespace osc {

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
        explicit TimeTag(const uint64_t w): v(w) {}
        operator uint64_t() const { return v; }
        static TimeTag immediate() { return TimeTag(1); }
    };

    struct Storage
    {
        Blob data;

        Storage() { data.reserve(200); }

        char* getBytes(const size_t sz)
        {
#ifndef ARDUINOOSC_DISABLE_STL
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

} // namespace osc
} // namespac arduino


using OscBlob = arduino::osc::Blob;
using OscTimeTag = arduino::osc::TimeTag;


#endif // ARDUINOOSC_OSCTYPES_H
