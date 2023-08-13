#pragma once
#ifndef ARDUINOOSCETH_H
#define ARDUINOOSCETH_H

#define ARDUINOOSC_ENABLE_ETH

#include <ETH.h>
#include <WiFiUdp.h>
#include "ArduinoOSC/ArduinoOSCCommon.h"
using OscEtherManager = ArduinoOSC::Manager<WiFiUDP>;
#define OscEther OscEtherManager::getInstance()
using OscEtherServer = OscServer<WiFiUDP>;
using OscEtherClient = OscClient<WiFiUDP>;

#endif  // ARDUINOOSCETH_H

