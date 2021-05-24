#pragma once
#ifndef ARDUINOOSCETHER_H
#define ARDUINOOSCETHER_H

#define ARDUINOOSC_ENABLE_ETHER

#include <Ethernet.h>
#include <EthernetUdp.h>
#include "ArduinoOSC/util/TeensyDirtySTLErrorSolution/TeensyDirtySTLErrorSolution.h"
#include "ArduinoOSC/ArduinoOSCCommon.h"
using OscEtherManager = ArduinoOSC::Manager<EthernetUDP>;
#define OscEther OscEtherManager::getInstance()
using OscEtherServer = OscServer<EthernetUDP>;
using OscEtherClient = OscClient<EthernetUDP>;

#endif  // ARDUINOOSCETHER_H
