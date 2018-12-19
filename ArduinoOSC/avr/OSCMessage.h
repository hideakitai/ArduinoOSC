#ifndef ARDUINOOSC_OSCMESSAGE_H
#define ARDUINOOSC_OSCMESSAGE_H

#include "OSCcommon.h"
#include "OSCArg.h"


class OSCMessage
{
public:

    OSCMessage()
    : ip_() , port_()
    , osc_addr_(), osc_addr_len_()
    , type_tag_len_()
    , args_size_() , args_len_()
    {}
    OSCMessage(const char *_oscAddr)
    : ip_() , port_()
    , osc_addr_(), osc_addr_len_()
    , type_tag_len_()
    , args_size_() , args_len_()
    {
        setOSCAddress(_oscAddr);
    }

    void flush()
    {
        osc_addr_.remove(0, osc_addr_.length());
        osc_addr_len_ = 0;
        args_size_ = 0;
        args_len_ = 0;
        port_ = 0;
    }

    // void beginMessage(const IPAddress& ip, uint16_t port)
    // {
    //     flush();
    //     setIpAddress(ip);
    //     setPortNumber(port);
    // }

    void beginMessage(const char* ip, uint16_t port)
    {
        flush();
        setIpAddress(ip);
        setPortNumber(port);
    }

    void beginMessage()
    {
        flush();
    }

    void setIpAddress(const char* ip)
    {
        ip_str_ = ip;
        String ip_str(ip);
        size_t start = 0;
        for (size_t i = 0; i < 3; ++i)
        {
            for (size_t j = start; j < ip_str.length(); ++j)
            {
                if (ip_str.charAt(j) == '.')
                {
                    ip_[i] = ip_str.substring(start, j).toInt();
                    start = j + 1;
                    break;
                }
            }
        }
        ip_[3] = ip_str.substring(start, ip_str.length()).toInt();
    }
    // void setIpAddress(const IPAddress& ip) { ip_ = ip; }
    // void setIpAddress(uint8_t ip0, uint8_t ip1, uint8_t ip2, uint8_t ip3) { ip_ = IPAddress(ip0, ip1, ip2, ip3); }
    void setIpAddress(uint8_t ip0, uint8_t ip1, uint8_t ip2, uint8_t ip3) { ip_[0] = ip0; ip_[1] = ip1; ip_[2] = ip2; ip_[3] = ip3; }
    void setPortNumber(uint16_t port) { port_ = port; }


    int16_t setOSCAddress(const char *addr_str)
    {
        String addr(addr_str);
        if(addr.length() > kMaxOSCAdrChar) { flush(); return -1; }
        osc_addr_ = addr;
        osc_addr_len_ = CULC_ALIGNMENT(osc_addr_.length());
        return 1;
    }

    int16_t addArgInt32(const int32_t& value)
    {
        if(args_size_ >= kMaxOSCArgSize) return -1;
        FourByteValue tmpValue;
        uint8_t *data = (uint8_t*)&value;
        swap(tmpValue.b, data);
        setArgData(kTagInt32, &(tmpValue.i), 4, false);
        return 1;
    }

    int16_t addArgFloat(const float& value)
    {
        if( args_size_ >= kMaxOSCArgSize) return -1;

        FourByteValue tmpValue;
        uint8_t *data = (uint8_t*)&value;
        swap(tmpValue.b, data);

        setArgData(kTagFloat, &(tmpValue.f), 4 , false );

        return 1;
    }

    int16_t addArgString(const char* value)
    {
        if (args_size_ >= kMaxOSCArgSize) return -1;
        setArgData( kTagString , (void*)value , strlen(value) , true );
        return 1;
    }
    int16_t addArgString(const String& str)
    {
        if (args_size_ >= kMaxOSCArgSize) return -1;
        setArgData(kTagString , (void*)str.c_str(), str.length(), true);
        return 1;
    }

    // const IPAddress& getIpAddress(void) { return ip_; }
    const char* getIpAddress() { return ip_str_; }
    uint16_t getPortNumber() { return port_; }

    const char*  getOSCAddress(void) { return osc_addr_.c_str(); }
    uint16_t getOSCAddrSize() { return osc_addr_.length(); }
    uint16_t getAddrAlignmentSize() { return osc_addr_len_; }

    uint16_t getTypeTagAlignmentSize() { return type_tag_len_; }

    uint16_t getNumArgs() { return args_size_; }
    char getArgTypeTag(uint16_t index)
    {
        if(index >= args_size_) return -1;
        return args_[index].getTypeTag();
    }

    OSCArg& getArg(size_t i) { return args_[i]; }
    uint16_t getArgAlignmentSize(uint8_t index) { return args_[index].getAlignmentSize(); }
    uint16_t getMessageSize(void)
    {
        return osc_addr_len_ + type_tag_len_ + args_len_;
    }


    int32_t getArgAsInt32(uint16_t index)
    {
        if (index >= args_size_) return -1;
        FourByteValue tmpValue;
        uint8_t *_bin = (uint8_t*)args_[index].getArgData();
        swap(tmpValue.b, _bin);
        return tmpValue.i;
    }

    float getArgAsFloat(uint16_t index)
    {
        if (index >= args_size_) return -1;
        FourByteValue tmpValue;
        uint8_t *_bin = (uint8_t*)args_[index].getArgData();
        swap(tmpValue.b, _bin);
        return tmpValue.f;
    }

    String getArgAsString(uint16_t index)
    {
        return String((const char*)args_[index].getArgData());
    }

    int16_t setArgData(char type, void *value, uint8_t byte,  bool enableAlignment)
    {
        args_[args_size_] = OSCArg(type, value, byte, enableAlignment);
        uint16_t alignSize = args_[args_size_].getAlignmentSize();
        args_len_ += alignSize;
        args_size_++;
        type_tag_len_ = CULC_ALIGNMENT(args_size_ + 1);
        return alignSize;
    }

private:

    union FourByteValue
    {
        uint8_t b[4];
        int32_t i;
        float f;
    };

    void swap(uint8_t *data1, uint8_t *data2)
    {
        data1[0] = data2[3];
        data1[1] = data2[2];
        data1[2] = data2[1];
        data1[3] = data2[0];
    }

    // IPAddress ip_;
    uint8_t ip_[4];
    uint16_t port_;
    const char* ip_str_;

	String   osc_addr_;
	uint16_t osc_addr_len_;

    uint16_t type_tag_len_;

    OSCArg  args_[kMaxOSCArgSize];
	uint16_t args_size_;
    uint16_t args_len_;
};


#endif // ARDUINOOSC_OSCMESSAGE_H
