#pragma once
#ifndef ARDUINOOSC_OSCSERVER_H
#define ARDUINOOSC_OSCSERVER_H

#include <ArxTypeTraits.h>
#include <ArxSmartPtr.h>
#include <ArxContainer.h>
#include <DebugLog.h>
#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
#include <cassert>
#endif

#include "OscMessage.h"
#include "OscDecoder.h"
#include "OscUdpMap.h"

namespace arduino {
namespace osc {
    namespace server {

        using namespace message;

        namespace element {
            struct Base {
                virtual ~Base() {}
                virtual void decodeFrom(Message& m, const size_t offset = 0) = 0;
            };

            template <typename T>
            inline void decode_from_msg(Message& m, const size_t i, T& t) {
                t = m.arg<T>(i);
            }

            inline void decode_from_msg(Message& m, const TupleRef& ts) {
                for (size_t idx = 0; idx < ts.size(); ++idx)
                    ts[idx]->decodeFrom(m, idx);
            }

            // single value
            template <typename T>
            class Value : public Base {
                T& t;

            public:
                Value(T& t)
                : t(t) {}
                virtual ~Value() {}
                virtual void decodeFrom(Message& m, const size_t offset = 0) override {
                    decode_from_msg(m, offset, t);
                }
            };

            // mutiple values
            class Tuple : public Base {
                TupleRef t;

            public:
                Tuple(TupleRef&& t)
                : t(std::move(t)) {}
                virtual ~Tuple() {}
                virtual void decodeFrom(Message& m, const size_t offset = 0) override {
                    (void)offset;
                    if (m.size() == t.size()) {
                        decode_from_msg(m, t);
                    } else {
                        LOG_ERROR("arg size mismatch: msg", m.size(), "/ subscribe", t.size());
                    }
                }
            };

            namespace detail {
                template <typename... Ts, size_t... Indices>
                inline void read_to_tuple(
                    std::index_sequence<Indices...>&&,
                    Message& m,
                    std::tuple<Ts...>& t) {
                    size_t o {0};
                    dummy_vector_t {(decode_from_msg(m, o, std::get<Indices>(t)), ++o)...};
                }

                template <typename... Ts>
                inline void read_to_tuple(Message& m, std::tuple<Ts...>& t) {
                    read_to_tuple(std::index_sequence_for<Ts...>(), m, t);
                }
            };  // namespace detail

            // callback with user defined arguments
            template <typename R, typename... Ts>
            class Function : public Base {
                using Func = std::function<R(Ts...)>;
                Func func;

            public:
                Function(Func func)
                : func(func) {};
                virtual ~Function() {}
                virtual void decodeFrom(Message& m, size_t offset = 0) override {
                    if (m.size() == sizeof...(Ts)) {
                        std::tuple<std::remove_cvref_t<Ts>...> t;
                        detail::read_to_tuple(m, t);
                        std::apply(func, t);
                        (void)offset;
                    } else {
                        LOG_ERROR("arg size mismatch: msg", m.size(), "/ func", sizeof...(Ts));
                    }
                }
            };

            // callback with Message&
            template <typename R>
            class Function<R, Message&> : public Base {
                using Func = std::function<R(Message&)>;
                Func func;

            public:
                Function(Func func)
                : func(func) {};
                virtual ~Function() {}
                virtual void decodeFrom(Message& m, size_t offset = 0) override {
                    func(m);
                    (void)offset;
                }
            };

            // callback with const Message&
            template <typename R>
            class Function<R, const Message&> : public Base {
                using Func = std::function<R(const Message&)>;
                Func func;

            public:
                Function(Func func)
                : func(func) {};
                virtual ~Function() {}
                virtual void decodeFrom(Message& m, size_t offset = 0) override {
                    func(m);
                    (void)offset;
                }
            };

        }  // namespace element

        // one (or last) argument
        template <typename T>
        inline auto make_element_ref(T& value)
            -> std::enable_if_t<!arx::is_callable<T>::value, ElementRef> {
            return ElementRef(new element::Value<T>(value));
        }

        // multiple parameters helper
        inline ElementRef make_element_ref(ElementTupleRef&& t) {
            return ElementRef(new element::Tuple(std::move(t)));
        }

        template <typename T, typename... Ts>
        inline auto make_element_ref(T&& t, Ts&&... ts)
            -> std::enable_if_t<!arx::is_callable<T>::value, ElementRef> {
            ElementTupleRef v {
                make_element_ref(std::forward<T>(t)),
                make_element_ref(std::forward<Ts>(ts))...};
            return make_element_ref(std::move(v));
        }

        // callbacks impl
        template <typename R, typename... Ts>
        inline ElementRef make_element_ref(std::function<R(Ts...)> func) {
            return ElementRef(new element::Function<R, Ts...>(func));
        }

        // functor i/f
        template <typename F>
        inline auto make_element_ref(F&& value)
            -> std::enable_if_t<arx::is_callable<F>::value, ElementRef> {
            return make_element_ref(arx::function_traits<F>::cast(value));
        }

        // function ptr i/f
        template <typename F>
        inline auto make_element_ref(F* value)
            -> std::enable_if_t<arx::is_callable<F>::value, ElementRef> {
            return make_element_ref(arx::function_traits<F>::cast(value));
        }

        template <typename S>
        class Server {
            Decoder decoder;
            CallbackMap callbacks;
            const uint16_t port;
            OscMessage* msg_ptr {nullptr};

        public:
            explicit Server(const uint16_t port)
            : port(port) {
                while (port == PORT_DISCARD) {
                    LOG_ERROR(F("Port #9 is not valid. Please change the server port."));
                    delay(1000);
                }
            }
            Server() {}

            template <typename... Ts>
            void subscribe(const String& addr, Ts&&... ts) {
                ElementRef ref = make_element_ref(std::forward<Ts>(ts)...);
                callbacks.insert({addr, ref});
            }

            bool unsubscribe(const String& addr) {
                auto it = callbacks.find(addr);
                if (it != callbacks.end()) {
                    callbacks.erase(it);
                    return true;
                }
                return false;
            }

            bool unsubscribeAll() {
                if (!callbacks.empty()) {
                    callbacks.clear();
                    return true;
                }
                return false;
            }

            bool parse() {
                auto stream = UdpMapManager<S>::getInstance().getUdp(port);
                const size_t size = stream->parsePacket();
                if (size == 0) return false;

                uint8_t data[size];
                stream->read(data, size);

                decoder.init(data, size);
                while (Message* msg = decoder.decode()) {
                    if (msg->available()) {
                        msg->remoteIP(stream->S::remoteIP());
                        msg->remotePort((uint16_t)stream->S::remotePort());
                        for (auto& c : this->callbacks) {
                            if (msg->match(c.first)) {
                                c.second->decodeFrom(*msg);
                            }
                        }
                        msg_ptr = msg;
                    } else {
                        LOG_ERROR(F("osc message parsing failed"));
                        msg_ptr = nullptr;
                    }
                }
                return msg_ptr != nullptr;
            }

            const OscMessage* message() const { return msg_ptr; }
        };

        template <typename S>
        class Manager {
            Manager() {}
            Manager(const Manager&) = delete;
            Manager& operator=(const Manager&) = delete;

            ServerMap<S> server_map;

        public:
            static Manager& getInstance() {
                static Manager m;
                return m;
            }

            const ServerMap<S>& getServerMap() const {
                return server_map;
            }

            Server<S>& getServer(const uint16_t port) {
                if (server_map.find(port) == server_map.end())
                    server_map.insert(std::make_pair(port, ServerRef<S>(new Server<S>(port))));
                return *(server_map[port].get());
            }

            template <typename... Ts>
            void subscribe(const uint16_t port, const String& addr, Ts&&... ts) {
                getServer(port).subscribe(addr, std::forward<Ts>(ts)...);
            }

            bool unsubscribe(const uint16_t port, const String& addr) {
                auto it = server_map.find(port);
                if (it != server_map.end()) {
                    return it->second->unsubscribe(addr);
                }
                return false;
            }

            bool unsubscribe(const uint16_t port) {
                auto it = server_map.find(port);
                if (it != server_map.end()) {
                    return it->second->unsubscribeAll();
                }
                return false;
            }

            bool unsubscribeAll() {
                if (!server_map.empty()) {
                    for (auto& m : server_map) {
                        m.second->unsubscribeAll();
                    }
                    return true;
                }
                return false;
            }

            void parse() {
                for (auto& m : server_map)
                    m.second->parse();
            }
        };

    }  // namespace server
}  // namespace osc
}  // namespace arduino

template <typename S>
using OscServer = arduino::osc::server::Server<S>;
template <typename S>
using OscServerManager = arduino::osc::server::Manager<S>;
template <typename S>
using OscServerMap = arduino::osc::server::ServerMap<S>;

#endif  // ARDUINOOSC_OSCSERVER_H
