#include <ArduinoOSC.h>

OscSerial osc;

void onOscReceived(OscMessage& m)
{
    osc.send("/callback/reply", m.arg<int>(0), m.arg<float>(1), m.arg<String>(2));
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
        osc.send("/lambda/reply", m.arg<int>(0), m.arg<float>(1), m.arg<String>(2));
    });

    osc.subscribe("/wildcard/*/test", [](OscMessage& m)
    {
        osc.send("/wildcard/reply", m.arg<int>(0));
    });

    // TODO: TBD
    // osc.publish(host, send_port, "/value", value);
    // osc.publish(host, send_port, "/millis", &millis);
}

void loop()
{
    osc.parse(); // should be called
}
