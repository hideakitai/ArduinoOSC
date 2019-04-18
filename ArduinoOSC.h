#ifndef ARDUINOOSC_H
#define ARDUINOOSC_H

#include "ArduinoOSC/OSCServer.h"
#include "ArduinoOSC/OSCClient.h"

#ifdef TEENSYDUINO // dirty
    namespace std
    {
        void __throw_bad_alloc() { _GLIBCXX_THROW_OR_ABORT(bad_alloc()); }
        void __throw_length_error(const char* __s __attribute__((unused))) { _GLIBCXX_THROW_OR_ABORT(length_error(_(__s))); }
        void __throw_bad_function_call() { _GLIBCXX_THROW_OR_ABORT(bad_function_call()); }
    }
#endif

namespace ArduinoOSC
{
    bool match(const String& pattern, const String& test, bool full = true)
    {
        if (full) return oscpkt::fullPatternMatch(pattern.c_str(), test.c_str());
        else      return oscpkt::partialPatternMatch(pattern.c_str(), test.c_str());
    }

    template <typename S>
    class ArduinoOSCUdp : public OscServerUdp<S>, public OscClientUdp<S>
    {
    public:
        void begin(uint32_t port)
        {
            stream.begin(port);
            this->OscServerUdp<S>::attach(stream);
            this->OscClientUdp<S>::attach(stream);
        }
    private:
        S stream;
    };

    class ArduinoOSCSerial : public OscServerSerial, public OscClientSerial
    {
    public:
        void attach(Stream& s)
        {
            stream = &s;
            this->OscServerSerial::attach(*stream);
            this->OscClientSerial::attach(*stream);
        }
    private:
        Stream* stream;
    };
}

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