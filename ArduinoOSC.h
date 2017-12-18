/*

 ArdOSC - OSC Library for Arduino.

 This library works with arduino firmware0018.

 2010/02/01 version 2.0 changed Project OSCClass -> ArdOSC
 2009/03/22 version 1.0.1 add errror process。change Doc.
 2009/03/21 version 1.0.0


 -------- Lisence -----------------------------------------------------------

 ArdOSC

 The MIT License

 Copyright (c) 2009 - 2010 recotana( http://recotana.com ) All right reserved

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.



 Thanks "Open Sound Control org"  http://opensoundcontrol.org/

 */

#ifndef ARDUINOOSC_H
#define ARDUINOOSC_H


#if defined (__AVR__)
    #include "Ethernet.h"
    // #include "ArduinoOSC/avr/OSCcommon.h"
    // #include "ArduinoOSC/avr/OSCMessage.h"
    // #include "ArduinoOSC/avr/OSCClient.h"
    // #include "ArduinoOSC/avr/OSCServer.h"
    #include "ArduinoOSC/OSCcommon.h"
    #include "ArduinoOSC/OSCMessage.h"
    #include "ArduinoOSC/OSCClient.h"
    #include "ArduinoOSC/OSCServer.h"
#elif defined (TEENSYDUINO) || defined (ESP_PLATFORM)
    #ifdef ESP_PLATFORM
        #include "WiFi.h"
        #include "WiFiUDP.h"
    #endif
    #include "ArduinoOSC/OSCcommon.h"
    #include "ArduinoOSC/OSCMessage.h"
    #include "ArduinoOSC/OSCClient.h"
    #include "ArduinoOSC/OSCServer.h"
#else
    #error UNSUPPORTED PLATFORM
#endif

template <typename S>
class ArduinoOSC : public OSCServer<S>, public OSCClient<S>
{
public:
    virtual ~ArduinoOSC() {}

    void begin(S& stream, uint32_t port)
    {
        stream.begin(port);
        OSCServer<S>::setup(stream);
        OSCClient<S>::setup(stream);
    }

};

#endif // ARDUINOOSC_H