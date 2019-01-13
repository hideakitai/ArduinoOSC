#include <ArduinoOSC.h>

OscSerial osc;

void onOscReceived(OscMessage& m)
{
    OscMessage msg;
    msg.init("/callback/reply");
    msg.push(m.getArgAsInt32(0)).push(m.getArgAsFloat(1)).push(m.getArgAsString(2));
    osc.send(msg);
}

void setup()
{
    // ArduinoOSC
    Serial.begin(115200);
    osc.attach(Serial);

    // TODO: TBD
    // osc.subscribe("/int32", i);
    // osc.subscribe("/float", f);
    // osc.subscribe("/string", s);
    // osc.subscribe("/blob", b);

    osc.subscribe("/callback", onOscReceived); // old style (v0.1.x)

    osc.subscribe("/lambda", [](OscMessage& m)
    {
        OscMessage msg;
        msg.init("/lambda/reply");
        msg.push(m.getArgAsInt32(0)).push(m.getArgAsFloat(1)).push(m.getArgAsString(2));
        osc.send(msg);
    });

    osc.subscribe("/wildcard/*/test", [](OscMessage& m)
    {
        OscMessage msg;
        msg.init("/wildcard/reply").push(m.getArgAsInt32(0));
        osc.send(msg);
    });

    // TODO: TBD
    // osc.publish(host, send_port, "/value", value);
    // osc.publish(host, send_port, "/millis", &millis);
}

void loop()
{
    osc.parse(); // should be called
}
