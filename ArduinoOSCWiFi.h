#pragma once
#ifndef ARDUINOOSCWIFI_H
#define ARDUINOOSCWIFI_H

#if defined(ESP_PLATFORM) || defined(ESP8266) || defined(ARDUINO_AVR_UNO_WIFI_REV2)                             \
    || defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_MKRVIDOR4000) || defined(ARDUINO_SAMD_MKR1000) \
    || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(ARDUINO_UNOR4_WIFI) \
    || defined(ARDUINO_GIGA) || defined(ARDUINO_RASPBERRY_PI_PICO_2W)
#define ARDUINOOSC_ENABLE_WIFI
#endif

#ifdef ARDUINOOSC_ENABLE_WIFI
#if defined(ESP_PLATFORM) || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_GIGA)
#include <WiFi.h>
#include <WiFiUdp.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#elif defined(ARDUINO_AVR_UNO_WIFI_REV2) || defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_MKRVIDOR4000) || defined(ARDUINO_SAMD_NANO_33_IOT)
#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#elif defined(ARDUINO_SAMD_MKR1000)
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#elif defined(ARDUINO_UNOR4_WIFI)
#include <WiFiS3.h>
#endif
#include "ArduinoOSC/ArduinoOSCCommon.h"
using OscWiFiManager = ArduinoOSC::Manager<WiFiUDP>;
#define OscWiFi OscWiFiManager::getInstance()
using OscWiFiServer = OscServer<WiFiUDP>;
using OscWiFiClient = OscClient<WiFiUDP>;
#endif  // ARDUINOOSC_ENABLE_WIFI

#endif  // ARDUINOOSCWIFI_H
