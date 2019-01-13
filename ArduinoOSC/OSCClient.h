#pragma once
#ifndef ARDUINOOSC_OSCCLIENT_H
#define ARDUINOOSC_OSCCLIENT_H

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
    using TimeTag = oscpkt::TimeTag;

    class OscWriter
    {
    public:
        OscWriter() { pw.init(); }
        OscWriter& init() { pw.init(); return *this; }
        OscWriter& encode(const OscMessage &msg) { pw.encode(msg); return *this; }
        uint32_t size() { return pw.size(); }
        const uint8_t* data() { return (const uint8_t*)pw.data(); }
        OscWriter& begin_bundle(TimeTag ts = TimeTag::immediate())
        {
            pw.begin_bundle(ts);
            return *this;
        }
        OscWriter& end_bundle()
        {
            pw.end_bundle();
            return *this;
        }
    private:
        oscpkt::PacketWriter pw;
    };


    template <typename S>
    class OscClient
    {
    public:
        virtual ~OscClient() {}
        void attach(S& s) { stream = &s; }
    protected:
        OscWriter writer;
        S* stream;
    };

    template <typename S>
    class OscClientUdp : public OscClient<S>
    {
    public:
        ~OscClientUdp() {}
#ifndef __AVR__
        template <typename... Rest>
        void send(const String& ip, uint16_t port, const String& addr, Rest&&... rest)
        {
            OscMessage msg(ip, port, addr);
            send(msg, std::forward<Rest>(rest)...);
        }
        template <typename First, typename... Rest>
        void send(OscMessage& msg, First&& first, Rest&&... rest)
        {
            msg.push(first);
            send(msg, std::forward<Rest>(rest)...);
        }
#endif
        void send(OscMessage& msg)
        {
            this->writer.init().encode(msg);
            this->stream->beginPacket(msg.ip().c_str(), msg.port());
            this->stream->write(this->writer.data(), this->writer.size());
            this->stream->endPacket();
        }
    };

    class OscClientSerial : public OscClient<Stream>
    {
    public:
        ~OscClientSerial() {}
#ifndef __AVR__
        template <typename... Rest>
        void send(const String& addr, Rest&&... rest)
        {
            OscMessage msg(addr);
            send(msg, std::forward<Rest>(rest)...);
        }
        template <typename First, typename... Rest>
        void send(OscMessage& msg, First&& first, Rest&&... rest)
        {
            msg.push(first);
            send(msg, std::forward<Rest>(rest)...);
        }
#endif
        void send(OscMessage& msg)
        {
            this->writer.init().encode(msg);
            this->packer.pack(this->writer.data(), this->writer.size());
            this->stream->write(this->packer.data(), this->packer.size());
        }
    private:
        #ifdef __AVR__
        Packetizer::Packer_<64> packer;
        #else
        Packetizer::Packer packer;
        #endif
    };
}

#endif // ARDUINOOSC_OSCCLIENT_H
