# ArduinoOSC

OSC library for Arduino (ESP, Teensy, AVR, etc.)

ArduinoOSC is OSC Library for Arduino, based on the great work [ArdOSC]().
Though [ArdOSC]() can only be used with EthernetShield, ArduinoEthernet or device with W5100, this library expands the supported streams to WiFi, Ethernet, Serial (TBD), and the others which are derived from Stream class.
For the [ArdOSC]() License, See ArduinoOSC/avr/Lisence.txt

## Usage

### via ```WiFiUdp```

``` c++
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOSC.h>

WiFiUDP udp;
ArduinoOSC<WiFiUDP> osc;

const char* ssid = "your-ssid";
const char* pwd = "your-password";
const char* host = "xxx.xxx.xxx.xxx";
const int recv_port = 10000;
const int send_port = 12000;

void setup()
{
    WiFi.begin(ssid, pwd);
    osc.begin(udp, recv_port);
    osc.addCallback("/ard/aaa", &callback);
}

void loop()
{
    osc.parse();
}

void callback(OSCMessage& m)
{
    //create new osc message
    OSCMessage msg;
    msg.beginMessage(sourceIp, send_port);

    // read & set same argument
    msg.setOSCAddress("/ard/aaa");
    msg.addArgInt32(m.getArgAsInt32(0));
    msg.addArgFloat(m.getArgAsFloat(1));
    msg.addArgString(m.getArgAsString(2));

    // send osc message
    osc.send(msg);
}
```

### via ```Serial```

```c++
#include "ArduinoOSC.h"
ArduinoOSCSerial osc;

void setup()
{
    osc.begin(Serial, 115200);
    osc.addCallback("/ard/aaa", &callback);
    delay(5000);
}

void loop()
{
    osc.parse();
}

void callback(OSCMessage& m)
{
    //create new osc message
    OSCMessage msg;
    msg.beginMessage();

    // read & set same argument
    msg.setOSCAddress(m.getOSCAddress());
    msg.addArgInt32(m.getArgAsInt32(0));
    msg.addArgFloat(m.getArgAsFloat(1));
    msg.addArgString(m.getArgAsString(2));

    //send osc message
    osc.send(msg);
}
```

## Supported Communication Library

- WiFiUdp (ESP)
- EthernetUDP
- Serial (Max7 example included)


## Tested Platform

- EPS32 w/ arduino-esp32
- Teensy 3.2, 3.5, 3.6
- Arduino Uno


## Limitation for AVR Platforms

- OSC packet size is limited to 128 bytes
- OSC type is limited only to INT32, FLOAT, STRING
- OSC argument size is limited to 15

currently, other platforms are also limited (because expansion is not applied)



## TODO

- re-write library and support NON-AVR platform for more safety and flexible use
- more examples

## License

MIT