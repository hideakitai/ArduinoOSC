#ifndef ARDUINOOSC_H
#define ARDUINOOSC_H

#include "ArduinoOSC/OSCServer.h"
#include "ArduinoOSC/OSCClient.h"

#include "ArduinoOSC/util/TeensyDirtySTLErrorSolution/TeensyDirtySTLErrorSolution.h"

namespace ArduinoOSC = arduino::osc;

namespace arduino {
namespace osc {

    inline bool match(const String& pattern, const String& test, bool full = true)
    {
        if (full) return oscpkt::fullPatternMatch(pattern.c_str(), test.c_str());
        else      return oscpkt::partialPatternMatch(pattern.c_str(), test.c_str());
    }

    template <typename S>
    class ArduinoOSCUdp
    : public OscServerUdp<S>, public OscClientUdp<S>
    {
        S stream;
    public:
        void begin(uint32_t port)
        {
            stream.begin(port);
            this->OscServerUdp<S>::attach(stream);
            this->OscClientUdp<S>::attach(stream);
        }
    };

    class ArduinoOSCSerial
    : public OscServerSerial, public OscClientSerial
    {
        Stream* stream;
    public:
        void attach(Stream& s)
        {
            stream = &s;
            this->OscServerSerial::attach(*stream);
            this->OscClientSerial::attach(*stream);
        }
    };

} // osc
} // arduino

namespace ArduinoOSC = arduino::osc;

#if defined (ESP_PLATFORM) || defined (ESP8266)
    using OscWiFi = ArduinoOSC::ArduinoOSCUdp<WiFiUDP>;
#elif defined (TEENSYDUINO) || defined (__AVR__)
    using OscEthernet = ArduinoOSC::ArduinoOSCUdp<EthernetUDP>;
#endif
using OscSerial = ArduinoOSC::ArduinoOSCSerial;
using OscMessage = ArduinoOSC::OscMessage;
using OscReader = ArduinoOSC::OscReader;
using OscWriter = ArduinoOSC::OscWriter;

#endif // ARDUINOOSC_H