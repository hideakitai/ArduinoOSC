#ifndef ARDUINOOSC_OSCCLIENT_H
#define ARDUINOOSC_OSCCLIENT_H

#include "OSCcommon.h"
#include "OSCMessage.h"
#include "Packetizer.h"

template <typename S>
class OSCClient
{

public:

    virtual ~OSCClient() {}

    bool begin(S& stream, uint16_t port)
    {
        setup(stream);
        return stream_->begin(port);
    }

    void setup(S& s) { stream_ = &s; }

    int16_t send(OSCMessage& msg);


private:

    int16_t encode(OSCMessage& msg, uint8_t *binData)
    {
        uint8_t *packStartPtr = binData;

        // OSC Address(String) -> BIN Encode
        memcpy(binData, msg.getOSCAddress(), msg.getOSCAddrSize());
        packStartPtr += msg.getAddrAlignmentSize();

        // TypeTag(String) -> BIN Encode
        *packStartPtr = ',';
        for ( uint8_t i=0 ; i<msg.getNumArgs(); i++ ) packStartPtr[i+1] = msg.getArgTypeTag(i);
        packStartPtr += msg.getTypeTagAlignmentSize();

        // Auguments -> BIN Encode
        for ( uint8_t i=0 ; i < msg.getNumArgs(); i++ )
        {
            switch (msg.getArgTypeTag(i))
            {
                case kTagInt32:
                case kTagFloat:
                case kTagString:
                {
                    memcpy(packStartPtr, msg.getArg(i).getArgData(), msg.getArgAlignmentSize(i));
                    break;
                }
                default:
                {
                    break;
                }
            }
            packStartPtr += msg.getArgAlignmentSize(i);
        }
        return 1;
    }

    uint8_t sendData[kMaxOSCPacketSize];
    Packetizer::Packer packer;
    S* stream_;
};

#if defined(TEENSYDUINO)
template <>
int16_t OSCClient<usb_serial_class>::send(OSCMessage& msg)
{
    memset(sendData, 0, kMaxOSCPacketSize);

    if(encode(msg, sendData) < 0) return -1;

    packer.pack(sendData, msg.getMessageSize());
    stream_->write(packer.data(), packer.size());

    return 0;
}

#elif defined(ESP_PLATFORM) || defined(ESP8266) || defined(__AVR__)

template <>
int16_t OSCClient<HardwareSerial>::send(OSCMessage& msg)
{
    memset(sendData, 0, kMaxOSCPacketSize);

    if(encode(msg, sendData) < 0) return -1;

    packer.pack(sendData, msg.getMessageSize());
    stream_->write(packer.data(), packer.size());

    return 0;
}

#if defined (ESP_PLATFORM) || defined (ESP8266)
template <>
int16_t OSCClient<WiFiUDP>::send(OSCMessage& msg)
{
    memset(sendData, 0, kMaxOSCPacketSize);

    if(encode(msg, sendData) < 0) return -1;

    stream_->beginPacket(msg.getIpAddress(), msg.getPortNumber());
    stream_->write(sendData, msg.getMessageSize());
    stream_->endPacket();

    return 0;
}

#endif
#endif


#endif // ARDUINOOSC_OSCCLIENT_H
