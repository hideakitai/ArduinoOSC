#include <ArduinoOSC.h>

// Ethernet stuff
uint8_t mac[] = {0xAB, 0xCD, 0xEF, 0x01, 0x23, 0x45};
const IPAddress ip (192, 168, 1, 201);
const IPAddress dns (192, 168, 1, 1);
const IPAddress gateway (192, 168, 1, 1);
const IPAddress subnet (255, 255, 255, 0);

// for ArduinoOSC
const char* host = "192.168.1.200";
const int recv_port = 54321;
const int bind_port = 54345;
const int send_port = 55555;
const int publish_port = 54445;
// send / receive varibales
int i;
float f;
String s;

void onOscReceived(OscMessage& m)
{
    Serial.print(m.remoteIP()); Serial.print(" ");
    Serial.print(m.remotePort()); Serial.print(" ");
    Serial.print(m.size()); Serial.print(" ");
    Serial.print(m.address()); Serial.print(" ");
    Serial.print(m.arg<int>(0)); Serial.print(" ");
    Serial.print(m.arg<float>(1)); Serial.print(" ");
    Serial.print(m.arg<String>(2)); Serial.println();
}

void setup()
{
    Serial.begin(115200);
    delay(2000);

    // Ethernet stuff
    Ethernet.begin(mac, ip);

    // Ethernet with useful options
    // Ethernet.begin(mac, ip, dns, gateway, subnet); // full
    // Ethernet.setRetransmissionCount(4); // default: 8[times]
    // Ethernet.setRetransmissionTimeout(50); // default: 200[ms]

    // publish osc messages (default publish rate = 30 [Hz])

    OscEther.publish(host, publish_port, "/publish/value", i, f, s)
        ->setFrameRate(60.f);

    OscEther.publish(host, publish_port, "/publish/func", &millis, &micros)
        ->setIntervalMsec(500.f);


    // subscribe osc messages

    OscEther.subscribe(bind_port, "/bind/values", i, f, s);

    OscEther.subscribe(bind_port, "/lambda/bind/args",
        [&](int& i, float& f, String& s)
        {
            Serial.print("/lambda/bind/args ");
            Serial.print(i); Serial.print(" ");
            Serial.print(f); Serial.print(" ");
            Serial.print(s); Serial.println();
        }
    );

    OscEther.subscribe(recv_port, "/lambda/msg",
        [](OscMessage& m)
        {
            Serial.print(m.remoteIP()); Serial.print(" ");
            Serial.print(m.remotePort()); Serial.print(" ");
            Serial.print(m.size()); Serial.print(" ");
            Serial.print(m.address()); Serial.print(" ");
            Serial.print(m.arg<int>(0)); Serial.print(" ");
            Serial.print(m.arg<float>(1)); Serial.print(" ");
            Serial.print(m.arg<String>(2)); Serial.println();
        }
    );

    OscEther.subscribe(recv_port, "/wildcard/*/test", [](OscMessage& m)
    {
        Serial.print(m.remoteIP()); Serial.print(" ");
        Serial.print(m.remotePort()); Serial.print(" ");
        Serial.print(m.size()); Serial.print(" ");
        Serial.print(m.address()); Serial.print(" ");
        Serial.print(m.arg<int>(0)); Serial.println();
    });

    OscEther.subscribe(recv_port, "/need/reply", []()
    {
        Serial.println("/need/reply");

        int i = millis();
        float f = (float)micros() / 1000.f;
        String s = "hello";

        OscEther.send(host, send_port, "/reply", i, f, s);
    });

    OscEther.subscribe(recv_port, "/callback", onOscReceived);

}

void loop()
{
    OscEther.update(); // should be called to receive + send osc

    // or do that separately
    // OscEther.parse(); // to receive osc
    // OscEther.post(); // to publish osc
}
