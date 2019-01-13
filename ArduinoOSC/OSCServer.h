#pragma once
#ifndef ARDUINOOSC_OSCSERVER_H
#define ARDUINOOSC_OSCSERVER_H

#ifdef ESP_PLATFORM
    #include <WiFi.h>
    #include <WiFiUdp.h>
#elif defined (ESP8266)
    #include <ESP8266WiFi.h>
    #include <WiFiUdp.h>
#elif defined (TEENSYDUINO) || defined (__AVR__)
    #include <Ethernet.h>
    #include <EthernetUdp.h>
#endif

#include "lib/oscpkt.hh"
#include "lib/Packetizer.h"

namespace ArduinoOSC
{
    using OscMessage = oscpkt::Message;
    using OscReader = oscpkt::PacketReader;

    template <typename S>
    class OscServer
    {
        #ifdef __AVR__
        typedef void (*CallbackType)(OscMessage& m);
        struct Map { String addr; CallbackType func; };
        using CallbackMap = RingBuffer<Map, 8>;
        #else
        using CallbackType = std::function<void(OscMessage& m)>;
        struct Map { String addr; CallbackType func; };
        using CallbackMap = std::vector<Map>;
        #endif
    public:
        virtual ~OscServer() {}
        void attach(S& s) { stream = &s; }
        void subscribe(const String& addr, CallbackType value) { callbacks.push_back({addr, value}); }

        virtual void parse() = 0;
    protected:
        OscReader reader;
        CallbackMap callbacks;
        S* stream;
    };

    template <typename S>
    class OscServerUdp : public OscServer<S>
    {
    public:
        virtual ~OscServerUdp() {}
        virtual void parse()
        {
            const size_t size = this->stream->parsePacket();
            if (size == 0) return;

            uint8_t data[size];
            this->stream->read(data, size);

            this->reader.init(data, size);
            if (OscMessage* msg = this->reader.decode())
            {
                if (msg->available())
                {
                    msg->ip(this->stream->S::remoteIP());
                    msg->port((uint16_t)this->stream->S::remotePort());
                    for (auto& c : this->callbacks) if (msg->match(c.addr)) c.func(*msg);
                }
                else
                {
                    Serial.println("message decode error invalid");
                }
            }
            else
            {
                Serial.println("message decode error nullptr");
            }
        }
    };

    class OscServerSerial : public OscServer<Stream>
    {
    public:
        virtual ~OscServerSerial() {}
        virtual void parse()
        {
            const size_t size = stream->available();
            if (size == 0) return;

            uint8_t data[size];
            stream->readBytes((char*)data, size);

            unpacker.feed(data, size);
            while (unpacker.available())
            {
                reader.init(unpacker.data(), unpacker.size());
                if (OscMessage* msg = reader.decode())
                    if (msg->available())
                        for (auto& c : callbacks)
                            if (msg->match(c.addr)) c.func(*msg);
                unpacker.pop();
            }
        }
    private:
        #ifdef __AVR__
        Packetizer::Unpacker_<1, 64, 0> unpacker;
        #else
        Packetizer::Unpacker unpacker;
        #endif
    };
}

#endif // ARDUINOOSC_OSCSERVER_H
