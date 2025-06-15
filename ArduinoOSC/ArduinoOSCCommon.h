#pragma once
#ifndef ARDUINOOSCCOMMON_H
#define ARDUINOOSCCOMMON_H

#include <ArxTypeTraits.h>
#include <ArxSmartPtr.h>
#include <ArxContainer.h>
#include <DebugLog.h>

#ifdef ARDUINOOSC_DEBUGLOG_ENABLE
#include <DebugLogEnable.h>
#else
#include <DebugLogDisable.h>
#endif

#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
// all features are available
#else
#ifndef ARDUINOOSC_ENABLE_BUNDLE
#define ARDUINOOSC_DISABLE_BUNDLE
#endif
#endif

#include "ArduinoOSC/OscUdpMap.h"
#include "ArduinoOSC/OSCServer.h"
#include "ArduinoOSC/OSCClient.h"

namespace arduino {
namespace osc {

    template <typename S>
    class Manager {
        Manager() {}
        Manager(const Manager&) = delete;
        Manager& operator=(const Manager&) = delete;

    public:
        // server

        static Manager<S>& getInstance() {
            static Manager<S> m;
            return m;
        }

        const OscUdpMap<S>& getUdpMap() const {
            return OscUdpMapManager<S>::getInstance().getUdpMap();
        }

        const OscServerMap<S>& getServerMap() const {
            return OscServerManager<S>::getInstance().getServerMap();
        }

        OscServer<S>& getServer(const uint16_t port) {
            return OscServerManager<S>::getInstance().getServer(port);
        }

        template <typename... Ts>
        void subscribe(const uint16_t port, const String& addr, Ts&&... ts) {
#if defined(ARDUINOOSC_ENABLE_WIFI) && (defined(ESP_PLATFORM) || defined(ARDUINO_ARCH_RP2040))
            if (WiFi.getMode() != WIFI_OFF)
                OscServerManager<S>::getInstance().getServer(port).subscribe(addr, std::forward<Ts>(ts)...);
            else
                LOG_ERROR(F("WiFi is not enabled. Subscribing OSC failed."));
#else
            OscServerManager<S>::getInstance().getServer(port).subscribe(addr, std::forward<Ts>(ts)...);
#endif
        }

        bool unsubscribe(const uint16_t port, const String& addr) {
#if defined(ARDUINOOSC_ENABLE_WIFI) && (defined(ESP_PLATFORM) || defined(ARDUINO_ARCH_RP2040))
            if (WiFi.getMode() != WIFI_OFF)
                return OscServerManager<S>::getInstance().unsubscribe(port, addr);
            else {
                LOG_ERROR(F("WiFi is not enabled. Unsubscribing OSC failed."));
                return false;
            }
#else
            return OscServerManager<S>::getInstance().unsubscribe(port, addr);
#endif
        }

        bool unsubscribe(const uint16_t port) {
#if defined(ARDUINOOSC_ENABLE_WIFI) && (defined(ESP_PLATFORM) || defined(ARDUINO_ARCH_RP2040))
            if (WiFi.getMode() != WIFI_OFF)
                return OscServerManager<S>::getInstance().unsubscribe(port);
            else {
                LOG_ERROR(F("WiFi is not enabled. Unsubscribing OSC failed."));
                return false;
            }
#else
            return OscServerManager<S>::getInstance().unsubscribe(port);
#endif
        }

        bool unsubscribe() {
#if defined(ARDUINOOSC_ENABLE_WIFI) && (defined(ESP_PLATFORM) || defined(ARDUINO_ARCH_RP2040))
            if (WiFi.getMode() != WIFI_OFF)
                return OscServerManager<S>::getInstance().unsubscribeAll();
            else {
                LOG_ERROR(F("WiFi is not enabled. Unsubscribing OSC failed."));
                return false;
            }
#else
            return OscServerManager<S>::getInstance().unsubscribeAll();
#endif
        }

        void parse() {
#if defined(ARDUINOOSC_ENABLE_WIFI) && (defined(ESP_PLATFORM) || defined(ARDUINO_ARCH_RP2040))
            if (this->isWiFiConnected() || this->isWiFiModeAP()) {
                OscServerManager<S>::getInstance().parse();
            } else {
                LOG_ERROR(F("WiFi is not connected. Please connected to WiFi"));
            }
#else
            OscServerManager<S>::getInstance().parse();
#endif
        }

        // client

        OscClient<S>& getClient() {
            return OscClientManager<S>::getInstance().getClient();
        }

        template <typename... Ts>
        void send(const String& ip, const uint16_t port, const String& addr, Ts&&... ts) {
#if defined(ARDUINOOSC_ENABLE_WIFI) && (defined(ESP_PLATFORM) || defined(ARDUINO_ARCH_RP2040))
            if (this->isWiFiConnected() || this->isWiFiModeAP()) {
                OscClientManager<S>::getInstance().send(ip, port, addr, std::forward<Ts>(ts)...);
            } else {
                LOG_ERROR(F("WiFi is not connected. Please connected to WiFi"));
            }
#else
            OscClientManager<S>::getInstance().send(ip, port, addr, std::forward<Ts>(ts)...);
#endif
        }

#ifndef ARDUINOOSC_DISABLE_BUNDLE

        void begin_bundle(const TimeTag &tt) {
            OscClientManager<S>::getInstance().begin_bundle(tt);
        }
        template <typename... Ts>
        void add_bundle(const String &addr, Ts&&... ts) {
            OscClientManager<S>::getInstance().add_bundle(addr, std::forward<Ts>(ts)...);
        }
        void end_bundle() {
            OscClientManager<S>::getInstance().end_bundle();
        }
        void send_bundle(const String& ip, const uint16_t port) {
#if defined(ARDUINOOSC_ENABLE_WIFI) && (defined(ESP_PLATFORM) || defined(ARDUINO_ARCH_RP2040))
            if (this->isWiFiConnected() || this->isWiFiModeAP()) {
                OscClientManager<S>::getInstance().send_bundle(ip, port);
            } else {
                LOG_ERROR(F("WiFi is not connected. Please connected to WiFi"));
            }
#else
            OscClientManager<S>::getInstance().send_bundle(ip, port);
#endif
        }

#endif // ARDUINOOSC_DISABLE_BUNDLE

        void post() {
#if defined(ARDUINOOSC_ENABLE_WIFI) && (defined(ESP_PLATFORM) || defined(ARDUINO_ARCH_RP2040))
            if (this->isWiFiConnected() || this->isWiFiModeAP()) {
                OscClientManager<S>::getInstance().post();
            } else {
                LOG_ERROR(F("WiFi is not connected. Please connected to WiFi"));
            }
#else
            OscClientManager<S>::getInstance().post();
#endif
        }

        template <typename... Ts>
        OscPublishElementRef publish(const String& ip, const uint16_t port, const String& addr, Ts&&... ts) {
#if defined(ARDUINOOSC_ENABLE_WIFI) && (defined(ESP_PLATFORM) || defined(ARDUINO_ARCH_RP2040))
            if (WiFi.getMode() != WIFI_OFF)
                return OscClientManager<S>::getInstance().publish(ip, port, addr, std::forward<Ts>(ts)...);
            else {
                LOG_ERROR(F("WiFi is not enabled. Publishing OSC failed."));
                return nullptr;
            }
#else
            return OscClientManager<S>::getInstance().publish(ip, port, addr, std::forward<Ts>(ts)...);
#endif
        }

        OscPublishElementRef getPublishElementRef(const String& ip, const uint16_t port, const String& addr) {
            return OscClientManager<S>::getInstance().getPublishElementRef(ip, port, addr);
        }

        // update both server and client

        void update() {
            parse();
            post();
        }

    private:
#if defined(ARDUINOOSC_ENABLE_WIFI) && (defined(ESP_PLATFORM) || defined(ARDUINO_ARCH_RP2040))
        bool isWiFiConnected() {
            return WiFi.status() == WL_CONNECTED;
        }

        bool isWiFiModeAP() {
            auto mode = WiFi.getMode();
            return (mode == WIFI_AP) || (mode == WIFI_AP_STA);
        }
#endif
    };

}  // namespace osc
}  // namespace arduino

namespace ArduinoOSC = arduino::osc;

#include <DebugLogRestoreState.h>

#endif  // ARDUINOOSCCOMMON_H
