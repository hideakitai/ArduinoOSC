#include "ArduinoOSC.h"

ArduinoOSC<HardwareSerial> osc; // Arduino Uno, ESP
// ArduinoOSC<usb_serial_class> osc; // teensy (via USB Serial)

void setup()
{
    osc.begin(Serial, 115200);
    osc.addCallback("/ard/aaa", &callback0);
    osc.addCallback("/test", &callback1);
}

void loop()
{
    osc.parse();
}

void callback0(OSCMessage& m)
{
    // get & send same osc data back

    //create new osc message
    OSCMessage msg;

    //set destination ip address & port no
    msg.beginMessage();

    //set argument
    msg.setOSCAddress(m.getOSCAddress());
    msg.addArgInt32(m.getArgAsInt32(0));
    msg.addArgFloat(m.getArgAsFloat(1));
    msg.addArgString(m.getArgAsString(2));

    //send osc message
    osc.send(msg);
}

void callback1(OSCMessage& m)
{
    // get & send same osc data back

    //create new osc message
    OSCMessage msg;

    //set destination ip address & port no
    msg.beginMessage();

    //set argument
    msg.setOSCAddress(m.getOSCAddress());
    msg.addArgInt32(m.getArgAsInt32(0));
    msg.addArgFloat(m.getArgAsFloat(1));
    msg.addArgString(m.getArgAsString(2));

    //send osc message
    osc.send(msg);
}


