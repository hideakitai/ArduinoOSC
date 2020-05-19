#pragma once
#ifndef ARDUINOOSC_OSCCLIENT_H
#define ARDUINOOSC_OSCCLIENT_H

#include "util/ArxTypeTraits/ArxTypeTraits.h"

#ifdef ARDUINOOSC_DISABLE_STL
    #include "util/ArxTypeTraits/ArxTypeTraits.h"
    #include "util/ArxSmartPtr/ArxSmartPtr.h"
    #include "util/ArxContainer/ArxContainer.h"
#else
    #include <memory>
    #include <vector>
    #include <map>
    #include <cassert>
#endif

#include "OscMessage.h"
#include "OscEncoder.h"

namespace arduino {
namespace osc {
namespace client {

    using namespace message;

    namespace element
    {
        struct Base
        {
            uint32_t last_publish_us {0};
            uint32_t interval_us {33333}; // 30 fps

            bool next() const { return micros() >= (last_publish_us + interval_us); }
            void setFrameRate(float fps) { interval_us = (uint32_t)(1000000.f / fps); }
            void setIntervalUsec(const uint32_t us) { interval_us = us; }
            void setIntervalMsec(const float ms) { interval_us = (uint32_t)(ms * 1000.f); }
            void setIntervalSec(const float sec) { interval_us = (uint32_t)(sec * 1000.f * 1000.f); }

            void init(Message& m, const String& addr) { m.init(addr); }

            virtual ~Base() {}
            virtual void encodeTo(Message& m) = 0;
        };

        template <typename T>
        class Value : public Base
        {
            T& t;
        public:
            Value(T& t) : t(t) {}
            virtual ~Value() {}
            virtual void encodeTo(Message& m) override { m.push(t); }
        };

        template <typename T>
        class Const : public Base
        {
            const T t;
        public:
            Const(const T& t) : t(t) {}
            virtual ~Const() {}
            virtual void encodeTo(Message& m) override { m.push(t); }
        };

        template <typename T>
        class Function : public Base
        {
            std::function<T()> getter;
        public:
            Function(const std::function<T()>& getter) : getter(getter) {}
            virtual ~Function() {}
            virtual void encodeTo(Message& m) override { m.push(getter()); }
        };

        class Tuple : public Base
        {
            TupleRef ts;
        public:
            Tuple(TupleRef&& ts) : ts(std::move(ts)) {}
            virtual ~Tuple() {}
            virtual void encodeTo(Message& m) override { for (auto& t : ts) t->encodeTo(m); }
        };

    } // namespace element


    template <typename T>
    inline auto make_element_ref(T& value)
    -> std::enable_if_t<!arx::is_callable<T>::value, ElementRef>
    {
        return ElementRef(new element::Value<T>(value));
    }

    template <typename T>
    inline auto make_element_ref(const T& value)
    -> std::enable_if_t<!arx::is_callable<T>::value, ElementRef>
    {
        return ElementRef(new element::Const<T>(value));
    }

    template <typename T>
    inline auto make_element_ref(const std::function<T()>& func)
    -> std::enable_if_t<!arx::is_callable<T>::value, ElementRef>
    {
        return ElementRef(new element::Function<T>(func));
    }

    template <typename Func>
    inline auto make_element_ref(const Func& func)
    -> std::enable_if_t<arx::is_callable<Func>::value, ElementRef>
    {
        return make_element_ref(arx::function_traits<Func>::cast(func));
    }

    // multiple parameters helper
    inline ElementRef make_element_ref(ElementTupleRef& t)
    {
        return ElementRef(new element::Tuple(std::move(t)));
    }


    struct Destination
    {
        String ip;
        uint16_t port;
        String addr;

        Destination(const Destination& dest)
        : ip(dest.ip), port(dest.port), addr(dest.addr) {}
        Destination(Destination&& dest)
        : ip(std::move(dest.ip)), port(std::move(dest.port)), addr(std::move(dest.addr)) {}
        Destination(const String& ip, const uint16_t port, const String& addr)
        : ip(ip), port(port), addr(addr) {}
        Destination() {}

        Destination& operator= (const Destination& dest)
        {
            ip = dest.ip;
            port = dest.port;
            addr = dest.addr;
            return *this;
        }
        Destination& operator= (Destination&& dest)
        {
            ip = std::move(dest.ip);
            port = std::move(dest.port);
            addr = std::move(dest.addr);
            return *this;
        }
        inline bool operator< (const Destination& rhs) const
        {
            return (ip != rhs.ip) ? (ip < rhs.ip) : (port != rhs.port) ? (port < rhs.port) : (addr.length() < rhs.addr.length());
        }
        inline bool operator== (const Destination& rhs) const
        {
            return (ip == rhs.ip) && (port == rhs.port) && (addr == rhs.addr);
        }
        inline bool operator!= (const Destination& rhs) const
        {
            return !(*this == rhs);
        }
    };


    template <typename S>
    class Client
    {
        Encoder writer;
        Message msg;
        S stream;
        static constexpr uint16_t PORT_DISCARD {9};

    public:

        Client() { stream.begin(PORT_DISCARD); }

        template <typename... Rest>
        void send(const String& ip, const uint16_t port, const String& addr, Rest&&... rest)
        {
            msg.init(addr);
            send(ip, port, msg, std::forward<Rest>(rest)...);
        }
        template <typename First, typename... Rest>
        void send(const String& ip, const uint16_t port, Message& m, First&& first, Rest&&... rest)
        {
            m.push(first);
            send(ip, port, m, std::forward<Rest>(rest)...);
        }
        void send(const String& ip, const uint16_t port, Message& m)
        {
            this->writer.init().encode(m);
            this->stream.beginPacket(ip.c_str(), port);
            this->stream.write(this->writer.data(), this->writer.size());
            this->stream.endPacket();
        }

        void send(const Destination& dest, ElementRef elem)
        {
            elem->init(msg, dest.addr);
            elem->encodeTo(msg);
            send(dest.ip, dest.port, msg);
        }
    };


    template <typename S>
    class Manager
    {
        Client<S> client;
        AddressMap addr_map;

    public:

        static Manager<S>& getInstance()
        {
            static Manager<S> m;
            return m;
        }

        Client<S>& getClient()
        {
            return client;
        }

        template <typename... Ts>
        void send(const String& ip, const uint16_t port, const String& addr, Ts&&... ts)
        {
            client.send(ip, port, addr, std::forward<Ts>(ts)...);
        }

        void post()
        {
            for (auto& mp : addr_map)
            {
                if (mp.second->next())
                {
                    mp.second->last_publish_us = micros();
                    client.send(mp.first, mp.second);
                }
            }
        }

        ElementRef publish(const String& ip, const uint16_t port, const String& addr, const char* const value)
        {
            return publish_impl(ip, port, addr, make_element_ref(value));
        }

        template <typename T>
        auto publish(const String& ip, const uint16_t port, const String& addr, T& value)
        -> std::enable_if_t<!arx::is_callable<T>::value, ElementRef>
        {
            return publish_impl(ip, port, addr, make_element_ref(value));
        }

        template <typename T>
        auto publish(const String& ip, const uint16_t port, const String& addr, const T& value)
        -> std::enable_if_t<!arx::is_callable<T>::value, ElementRef>
        {
            return publish_impl(ip, port, addr, make_element_ref(value));
        }

        template <typename Func>
        auto publish(const String& ip, const uint16_t port, const String& addr, Func&& func)
        -> std::enable_if_t<arx::is_callable<Func>::value, ElementRef>
        {
            return publish(ip, port, addr, arx::function_traits<Func>::cast(func));
        }

        template <typename T>
        ElementRef publish(const String& ip, const uint16_t port, const String& addr, std::function<T()>&& getter)
        {
            return publish_impl(ip, port, addr, make_element_ref(getter));
        }

        template <typename... Ts>
        ElementRef publish(const String& ip, const uint16_t port, const String& addr, Ts&&... ts)
        {
            ElementTupleRef v { make_element_ref(std::forward<Ts>(ts))... };
            return publish_impl(ip, port, addr, make_element_ref(v));
        }

        ElementRef getPublishElementRef(const String& ip, const uint16_t port, const String& addr)
        {
            Destination dest {ip, port, addr};
            return addr_map[dest];
        }

    private:

        ElementRef publish_impl(const String& ip, const uint16_t port, const String& addr, ElementRef ref)
        {
            Destination dest {ip, port, addr};
            addr_map.insert(make_pair(dest, ref));
            return ref;
        }
    };


} // namespace client
} // namespace osc
} // namespace arduino


template <typename S>
using OscClient = arduino::osc::client::Client<S>;
template <typename S>
using OscClientManager = arduino::osc::client::Manager<S>;
using OscPublishElementRef = arduino::osc::client::ElementRef;


#endif // ARDUINOOSC_OSCCLIENT_H
