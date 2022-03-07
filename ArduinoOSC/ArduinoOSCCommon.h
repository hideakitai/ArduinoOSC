#pragma once
#ifndef ARDUINOOSCCOMMON_H
#define ARDUINOOSCCOMMON_H

#include "ArduinoOSC/util/ArxTypeTraits/ArxTypeTraits.h"
#include "ArduinoOSC/util/ArxSmartPtr/ArxSmartPtr.h"
#include "ArduinoOSC/util/ArxContainer/ArxContainer.h"
#include "ArduinoOSC/util/DebugLog/DebugLog.h"

#ifdef ARDUINOOSC_DEBUGLOG_ENABLE
#include "ArduinoOSC/util/DebugLog/DebugLogEnable.h"
#else
#include "ArduinoOSC/util/DebugLog/DebugLogDisable.h"
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
#if defined(ARDUINOOSC_ENABLE_WIFI) && defined(ESP_PLATFORM)
            if (WiFi.getMode() != WIFI_OFF)
                OscServerManager<S>::getInstance().getServer(port).subscribe(addr, std::forward<Ts>(ts)...);
            else
                LOG_ERROR(F("WiFi is not enabled. Subscribing OSC failed."));
#else
            OscServerManager<S>::getInstance().getServer(port).subscribe(addr, std::forward<Ts>(ts)...);
#endif
        }

        void parse() {
#if defined(ARDUINOOSC_ENABLE_WIFI) && defined(ESP_PLATFORM)
            if ((WiFi.status() == WL_CONNECTED) || (WiFi.getMode() != WIFI_STA)) {
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
#if defined(ARDUINOOSC_ENABLE_WIFI) && defined(ESP_PLATFORM)
            if ((WiFi.status() == WL_CONNECTED) || (WiFi.getMode() != WIFI_STA)) {
                OscClientManager<S>::getInstance().send(ip, port, addr, std::forward<Ts>(ts)...);
            } else {
                LOG_ERROR(F("WiFi is not connected. Please connected to WiFi"));
            }
#else
            OscClientManager<S>::getInstance().send(ip, port, addr, std::forward<Ts>(ts)...);
#endif
        }

        void post() {
#if defined(ARDUINOOSC_ENABLE_WIFI) && defined(ESP_PLATFORM)
            if ((WiFi.status() == WL_CONNECTED) || (WiFi.getMode() != WIFI_STA)) {
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
#if defined(ARDUINOOSC_ENABLE_WIFI) && defined(ESP_PLATFORM)
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
    };

}  // namespace osc
}  // namespace arduino

namespace ArduinoOSC = arduino::osc;

#include "ArduinoOSC/util/DebugLog/DebugLogRestoreState.h"

#endif  // ARDUINOOSCCOMMON_H
