# ArduinoOSC
OSC library for Arduino (ESP, Teensy, etc.)

## This library is WIP

ArduinoOSC is OSC Library for Arduino, based on the great work [ArdOSC]().
Though [ArdOSC]() can only be used with EthernetShield, ArduinoEthernet or device with W5100, this library expands the supported streams to WiFi, Ethernet, Serial (TBD), and the others which are derived from Stream class.
For the [ArdOSC]() License, See ArduinoOSC/avr/Lisence.txt

## Usage

```
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
    Serial.begin(115200);
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
    // get osc data

    // remote ip address
    IPAddress sourceIp = m.getIpAddress();
    //get 1st argument(int32)
    Serial.print("arg 0 : ");
    Serial.println(m.getArgAsInt32(0));
    //get 2nd argument(float)
    Serial.print("arg 1 : ");
    Serial.println(m.getArgAsFloat(1));
    //get 3rd argument(string)
    Serial.print("arg 2 : ");
    Serial.println(m.getArgAsString(2));

    // send osc data

    //create new osc message
    OSCMessage msg;
    //set destination ip address & port no
    msg.beginMessage(sourceIp, send_port);
    //set argument
    msg.setOSCAddress("/ard/aaa");
    msg.addArgInt32(m.getArgAsInt32(0));
    msg.addArgFloat(m.getArgAsFloat(1));
    msg.addArgString(m.getArgAsString(2));
    //send osc message
    osc.send(msg);
}
```



## Notation

only loopback.ino with ESP32 module can be run in example folder



## Supported / Tested Application

- openFrameworks
- Cycling74 Max7
- Processing
- Pd
- etc.

## Supported Communication Library

- WiFiUdp (EPS)
- EthernetUDP (Arduino)
- Serial (not yet, see TODO)

## Supported Platform

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

- emulate OSC packet in Serial, with header, footer, and checker (e.g. CRC)

- more examples

  ​

## License

MIT