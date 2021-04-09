#pragma once

#ifndef ARDUINOOSC_OSCTYPES_H
#define ARDUINOOSC_OSCTYPES_H

#include <Arduino.h>
#include "util/ArxTypeTraits/ArxTypeTraits.h"
#include "util/ArxContainer/ArxContainer.h"
#include "util/DebugLog/DebugLog.h"

#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11

namespace arduino {
namespace osc {

    static constexpr uint16_t PORT_DISCARD {9};
    template <typename S>
    using UdpRef = std::shared_ptr<S>;
    template <typename S>
    using UdpMap = std::map<uint16_t, UdpRef<S>>;

    namespace message {
        using ArgumentType = std::pair<size_t, size_t>;
        using ArgumentQueue = std::vector<ArgumentType>;
        class Message;
        using MessageQueue = std::vector<Message>;
    }  // namespace message
    using BundleData = std::vector<uint32_t>;
    using Blob = std::vector<char>;

    namespace client {
        namespace element {
            class Base;
            using Ref = std::shared_ptr<Base>;
            using TupleRef = std::vector<Ref>;
        }  // namespace element
        class Destination;
        using ElementRef = element::Ref;
        using ElementTupleRef = element::TupleRef;
        using DestinationMap = std::map<Destination, ElementRef>;
    }  // namespace client

    namespace server {
        namespace element {
            class Base;
            using Ref = std::shared_ptr<Base>;
            using TupleRef = std::vector<Ref>;
            using dummy_vector_t = std::vector<size_t>;
        }  // namespace element
        using ElementRef = element::Ref;
        using ElementTupleRef = element::TupleRef;
        using CallbackMap = std::map<String, ElementRef>;
        template <typename S>
        class Server;
        template <typename S>
        using ServerRef = std::shared_ptr<Server<S>>;
        template <typename S>
        using ServerMap = std::map<uint16_t, ServerRef<S>>;
    }  // namespace server

}  // namespace osc
}  // namespace arduino

#else

namespace arduino {
namespace osc {

#ifndef ARDUINOOSC_MAX_MSG_ARGUMENT_SIZE
#define ARDUINOOSC_MAX_MSG_ARGUMENT_SIZE 8
#endif
#ifndef ARDUINOOSC_MAX_MSG_BYTE_SIZE
#define ARDUINOOSC_MAX_MSG_BYTE_SIZE 128
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
#define ARDUINOOSC_MAX_MSG_BUNDLE_SIZE 128
#endif

    static constexpr uint16_t PORT_DISCARD {9};
    template <typename S>
    using UdpRef = std::shared_ptr<S>;
    template <typename S>
    using UdpMap = arx::map<uint16_t, UdpRef<S>, ARDUINOOSC_MAX_SUBSCRIBE_PORTS>;

    namespace message {
        using ArgumentType = arx::pair<size_t, size_t>;
        using ArgumentQueue = arx::vector<ArgumentType, ARDUINOOSC_MAX_MSG_ARGUMENT_SIZE>;
        class Message;
        using MessageQueue = arx::vector<Message, ARDUINOOSC_MAX_MSG_QUEUE_SIZE>;
    }  // namespace message
#ifndef ARDUINOOSC_DISABLE_BUNDLE
    using BundleData = arx::vector<uint32_t, ARDUINOOSC_MAX_MSG_BUNDLE_SIZE>;
#endif
    using Blob = arx::vector<char, ARDUINOOSC_MAX_MSG_BYTE_SIZE>;

    namespace client {
        namespace element {
            class Base;
            using Ref = std::shared_ptr<Base>;
            using TupleRef = arx::vector<Ref, ARDUINOOSC_MAX_MSG_ARGUMENT_SIZE>;
        }  // namespace element
        class Destination;
        using ElementRef = element::Ref;
        using ElementTupleRef = element::TupleRef;
        using DestinationMap = arx::map<Destination, ElementRef, ARDUINOOSC_MAX_PUBLISH_DESTINATION>;
    }  // namespace client

    namespace server {
        namespace element {
            class Base;
            using Ref = std::shared_ptr<Base>;
            using TupleRef = arx::vector<Ref, ARDUINOOSC_MAX_MSG_ARGUMENT_SIZE>;
            using dummy_vector_t = arx::vector<size_t, ARDUINOOSC_MAX_MSG_ARGUMENT_SIZE>;
        }  // namespace element
        using ElementRef = element::Ref;
        using ElementTupleRef = element::TupleRef;
        using CallbackMap = arx::map<String, ElementRef, ARDUINOOSC_MAX_SUBSCRIBE_ADDRESS_PER_PORT>;
        template <typename S>
        class Server;
        template <typename S>
        using ServerRef = std::shared_ptr<Server<S>>;
        template <typename S>
        using ServerMap = arx::map<uint16_t, ServerRef<S>, ARDUINOOSC_MAX_SUBSCRIBE_PORTS>;
    }  // namespace server

}  // namespace osc
}  // namespace arduino

#endif

#include "OscUtil.h"

namespace arduino {
namespace osc {

    enum {
        TYPE_TAG_TRUE = 'T',
        TYPE_TAG_FALSE = 'F',
        TYPE_TAG_INT32 = 'i',
        TYPE_TAG_INT64 = 'h',
        TYPE_TAG_FLOAT = 'f',
        TYPE_TAG_DOUBLE = 'd',
        TYPE_TAG_STRING = 's',
        TYPE_TAG_BLOB = 'b'
    };

    class TimeTag {
        uint64_t v;

    public:
        TimeTag()
        : v(1) {}
        explicit TimeTag(const uint64_t w)
        : v(w) {}
        operator uint64_t() const { return v; }
        static TimeTag immediate() { return TimeTag(1); }
    };

    struct Storage {
        Blob data;

        // reserve() makes no sense on arx::vector because capacity is fixed
        Storage() { data.reserve(200); }

        char* getBytes(const size_t sz) {
            if ((data.size() & 3) != 0) {
                LOG_ERROR(F("storage size must be 4x bytes but it is"), data.size());
                return nullptr;
            }
#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
            if (data.size() + sz > data.capacity()) {
                data.reserve((data.size() + sz) * 2);
            }
#else
            if (data.size() + sz > data.capacity()) {
                LOG_ERROR(F("storage size overflow:"), data.size() + sz, F("must be <="), data.capacity());
                return nullptr;
            }
#endif
            size_t sz4 = ceil4(sz);
            size_t pos = data.size();
            data.resize(pos + sz4);  // resize will fill with zeros, so the zero padding is OK
            return &(data[pos]);
        }
        char* begin() { return data.size() ? &data.front() : nullptr; }
        char* end() { return begin() ? (begin() + size()) : nullptr; }
        const char* begin() const { return data.size() ? &data.front() : nullptr; }
        const char* end() const { return begin() ? (begin() + size()) : nullptr; }
        size_t size() const { return data.size(); }
        void assign(const char* beg, const char* end) { data.assign(beg, end); }
        void clear() { data.clear(); }
    };

}  // namespace osc
}  // namespace arduino

using OscBlob = arduino::osc::Blob;
using OscTimeTag = arduino::osc::TimeTag;

#endif  // ARDUINOOSC_OSCTYPES_H
