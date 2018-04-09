#ifndef ARDUINOOSC_OSCSERVER_H
#define ARDUINOOSC_OSCSERVER_H

#include "Pattern.h"
#include "Packetizer.h"

template <typename S>
class OSCServer
{
public:

    virtual ~OSCServer() {}

    bool begin(S& stream, uint16_t port)
    {
        setup(stream);
        return stream_->begin(port);
    }

    void setup(S& stream) { stream_ = &stream; }

    int16_t parse();

    void addCallback(const char* adr , Pattern::AdrFunc func )
    {
        adrMatch_.addOscAddress(adr, func);
    }

private:

    // TODO: wait for all packets when it is Serial, because sometimes packets are devided into some pieces...
    int16_t decode(OSCMessage& m, const uint8_t* binData)
    {
        const uint8_t *packStartPtr = binData;
        m.setOSCAddress((char*)binData);
        packStartPtr += m.getAddrAlignmentSize();
        char *tmpTag = (char*)(packStartPtr + 1);
        uint8_t argsNum = strlen(tmpTag);
        uint16_t typeTagAlignSize = CULC_ALIGNMENT(argsNum+1);
        packStartPtr += typeTagAlignSize;

        for (uint8_t i = 0 ; i < argsNum ; ++i)
        {
            switch (tmpTag[i])
            {
                case kTagInt32:
                {
                    packStartPtr += m.setArgData( kTagInt32 , (void*)packStartPtr , 4 , false );
                    break;
                }

                case kTagFloat:
                {
                    packStartPtr += m.setArgData( kTagFloat , (void*)packStartPtr , 4 , false );
                    break;
                }
                case kTagString:
                {
                    packStartPtr += m.setArgData( kTagString , (void*)packStartPtr , strlen((char*)packStartPtr) , true );
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
        return 1;
    }

    Pattern adrMatch_;
    Packetizer::Unpacker unpacker;
    S* stream_;
};

#if defined (TEENSYDUINO)

template <>
int16_t OSCServer<usb_serial_class>::parse()
{
    const size_t size = stream_->available();
    if (size == 0) return 0;

    OSCMessage rcvMes;

    uint8_t data[size];
    stream_->readBytes((char*)data, size);

    unpacker.feed(data, size);

    while (unpacker.available())
    {
        if (decode(rcvMes, unpacker.data()) >= 0)
        {
            adrMatch_.paternComp(rcvMes);
        }
        unpacker.pop();
    }

    return 0;
}

#elif defined (ESP_PLATFORM) || defined (ESP8266) || defined(__AVR__)

template <>
int16_t OSCServer<HardwareSerial>::parse()
{
    const size_t size = stream_->available();
    if (size == 0) return 0;

    OSCMessage rcvMes;

    uint8_t data[size];
    stream_->readBytes((char*)data, size);

    unpacker.feed(data, size);

    while (unpacker.available())
    {
        if (decode(rcvMes, unpacker.data()) >= 0)
        {
            adrMatch_.paternComp(rcvMes);
        }
        unpacker.pop();
    }

    return 0;
}

#if defined (ESP_PLATFORM) || defined (ESP8266)
template <>
int16_t OSCServer<WiFiUDP>::parse()
{
    const size_t size = stream_->parsePacket();
    if (size == 0) return 0;

    OSCMessage rcvMes;
    // rcvMes.setIpAddress(stream_->remoteIP());
    // rcvMes.setPortNumber(stream_->remotePort());

    uint8_t data[size];
    stream_->read(data, size);

    if (decode(rcvMes, data) < 0) return -1;
    adrMatch_.paternComp(rcvMes);
    return size;
}

#endif
#endif

#endif // ARDUINOOSC_OSCSERVER_H
