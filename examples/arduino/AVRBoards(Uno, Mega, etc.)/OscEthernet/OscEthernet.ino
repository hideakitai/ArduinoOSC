#include <ArduinoOSC.h>

// Ethernet stuff
const IPAddress ip(192, 168, 1, 201);
uint8_t mac[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB};

// for ArduinoOSC
OscEthernet osc;
const char* host = "192.168.1.200";
const int recv_port = 10000;
const int send_port = 12000;

void onOscReceived(OscMessage& m)
{
    Serial.print("callback : ");
    Serial.print(m.ip()); Serial.print(" ");
    Serial.print(m.port()); Serial.print(" ");
    Serial.print(m.size()); Serial.print(" ");
    Serial.print(m.address()); Serial.print(" ");
    Serial.print(m.getArgAsInt32(0)); Serial.print(" ");
    Serial.print(m.getArgAsFloat(1)); Serial.print(" ");
    Serial.print(m.getArgAsString(2)); Serial.println();
}

void setup()
{
    Serial.begin(115200);

    // Ethernet stuff
    Ethernet.begin(mac, ip);

    // ArduinoOSC
    osc.begin(recv_port);

    // TODO: TBD
    // osc.subscribe("/int32", i);
    // osc.subscribe("/float", f);
    // osc.subscribe("/string", s);
    // osc.subscribe("/blob", b);

    osc.subscribe("/callback", onOscReceived); // old style (v0.1.x)

    osc.subscribe("/lambda", [](OscMessage& m)
    {
        Serial.print("lambda : ");
        Serial.print(m.ip()); Serial.print(" ");
        Serial.print(m.port()); Serial.print(" ");
        Serial.print(m.size()); Serial.print(" ");
        Serial.print(m.address()); Serial.print(" ");
        Serial.print(m.getArgAsInt32(0)); Serial.print(" ");
        Serial.print(m.getArgAsFloat(1)); Serial.print(" ");
        Serial.print(m.getArgAsString(2)); Serial.println();
    });
    osc.subscribe("/wildcard/*/test", [](OscMessage& m)
    {
        Serial.print("wildcard : ");
        Serial.print(m.ip()); Serial.print(" ");
        Serial.print(m.port()); Serial.print(" ");
        Serial.print(m.size()); Serial.print(" ");
        Serial.print(m.address()); Serial.print(" ");
        Serial.print(m.getArgAsInt32(0)); Serial.println();
    });
    osc.subscribe("/need/reply", [](OscMessage& m)
    {
        Serial.println("need reply :");

        int i = 12;
        float f = 34.56F;
        String s = "hello";

        OscMessage msg(host, send_port, "/send");
        msg.push(i).push(f).push(s);
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
