#pragma once

#include <Arduino.h>
#include <ArduinoOSCWiFi.h>

class OSCManager
{
public:
    OSCManager();
    ~OSCManager() = default;

    static void onOscReceived(OscMessage &m);
};
