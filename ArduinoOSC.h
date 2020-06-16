#ifndef ARDUINOOSC_H
#define ARDUINOOSC_H

#if defined(ARDUINO_ARCH_AVR)\
 || defined(ARDUINO_ARCH_MEGAAVR)\
 || defined(ARDUINO_ARCH_SAMD)\
 || defined(ARDUINO_spresense_ast)
    #define ARDUINOOSC_DISABLE_STL
    #ifndef ARDUINOOSC_ENABLE_BUNDLE
        #define ARDUINOOSC_DISABLE_BUNDLE
    #endif
#endif

#if defined(ESP_PLATFORM)\
 || defined(ESP8266)\
 || defined(ARDUINO_AVR_UNO_WIFI_REV2)\
 || defined(ARDUINO_SAMD_MKRWIFI1010)\
 || defined(ARDUINO_SAMD_MKRVIDOR4000)\
 || defined(ARDUINO_SAMD_MKR1000)\
 || defined(ARDUINO_SAMD_NANO_33_IOT)
    #define ARDUINOOSC_ENABLE_WIFI
#endif

#if defined(TEENSYDUINO)\
 || defined(ESP8266)\
 || defined(ARDUINOOSC_DISABLE_STL)
    #define ARDUINOOSC_ENABLE_ETHER
#endif

#if !defined (ARDUINOOSC_ENABLE_WIFI)\
 && !defined (ARDUINOOSC_ENABLE_ETHER)\
 && !defined (ARDUINOOSC_DISABLE_STL)
    #error THIS PLATFORM HAS NO WIFI OR ETHERNET OR NOT SUPPORTED ARCHITECTURE. PLEASE LET ME KNOW!
#endif

#ifdef ARDUINOOSC_ENABLE_WIFI
    #ifdef ESP_PLATFORM
        #include <WiFi.h>
        #include <WiFiUdp.h>
    #elif defined (ESP8266)
        #include <ESP8266WiFi.h>
        #include <WiFiUdp.h>
    #elif defined (ARDUINO_AVR_UNO_WIFI_REV2)\
        || defined(ARDUINO_SAMD_MKRWIFI1010)\
        || defined(ARDUINO_SAMD_MKRVIDOR4000)\
        || defined(ARDUINO_SAMD_NANO_33_IOT)
        #include <SPI.h>
        #include <WiFiNINA.h>
        #include <WiFiUdp.h>
    #elif defined (ARDUINO_SAMD_MKR1000)
        #include <SPI.h>
        #include <WiFi101.h>
        #include <WiFiUdp.h>
    #endif
#endif // ARDUINOOSC_ENABLE_WIFI

#ifdef ARDUINOOSC_ENABLE_ETHER
    #include <Ethernet.h>
    #include <EthernetUdp.h>
    #include "ArduinoOSC/util/TeensyDirtySTLErrorSolution/TeensyDirtySTLErrorSolution.h"
#endif // ARDUINOOSC_ENABLE_ETHER


#include "ArduinoOSC/OSCServer.h"
#include "ArduinoOSC/OSCClient.h"


namespace arduino {
namespace osc {

    template <typename S>
    struct Manager
    {
        // server

        static Manager<S>& getInstance()
        {
            static Manager<S> m;
            return m;
        }

        const OscServerMap<S>& getServerMap() const
        {
            return OscServerManager<S>::getInstance().getServerMap();
        }

        OscServer<S>& getServer(const uint16_t port)
        {
            return OscServerManager<S>::getInstance().getServer(port);
        }

        template <typename... Ts>
        void subscribe(const uint16_t port, const String& addr, Ts&&... ts)
        {
            OscServerManager<S>::getInstance().getServer(port).subscribe(addr, std::forward<Ts>(ts)...);
        }

        void parse()
        {
            OscServerManager<S>::getInstance().parse();
        }


        // client

        OscClient<S>& getClient()
        {
            return OscClientManager<S>::getInstance().getClient();
        }

        template <typename... Ts>
        void send(const String& ip, const uint16_t port, const String& addr, Ts&&... ts)
        {
            OscClientManager<S>::getInstance().send(ip, port, addr, std::forward<Ts>(ts)...);
        }

        void post()
        {
            OscClientManager<S>::getInstance().post();
        }

        template <typename... Ts>
        OscPublishElementRef publish(const String& ip, const uint16_t port, const String& addr, Ts&&... ts)
        {
            return OscClientManager<S>::getInstance().publish(ip, port, addr, std::forward<Ts>(ts)...);
        }

        OscPublishElementRef getPublishElementRef(const String& ip, const uint16_t port, const String& addr)
        {
            return OscClientManager<S>::getInstance().getPublishElementRef(ip, port, addr);
        }


        // update both server and client

        void update()
        {
            parse();
            post();
        }

    };

} // osc
} // arduino


namespace ArduinoOSC = arduino::osc;

#ifdef ARDUINOOSC_ENABLE_WIFI
    using OscWiFiManager = ArduinoOSC::Manager<WiFiUDP>;
    #define OscWiFi OscWiFiManager::getInstance()
#endif // ARDUINOOSC_ENABLE_WIFI

#ifdef ARDUINOOSC_ENABLE_ETHER
    using OscEtherManager = ArduinoOSC::Manager<EthernetUDP>;
    #define OscEther OscEtherManager::getInstance()
#endif // ARDUINOOSC_ENABLE_ETHER


#endif // ARDUINOOSC_H
