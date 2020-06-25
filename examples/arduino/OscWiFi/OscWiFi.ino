#include <ArduinoOSC.h>

// WiFi stuff
const char* ssid = "your-ssid";
const char* pwd = "your-password";
const IPAddress ip(192, 168, 1, 201);
const IPAddress gateway(192, 168, 1, 1);
const IPAddress subnet(255, 255, 255, 0);

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

void onOscReceived(const OscMessage& m)
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

    // WiFi stuff (no timeout setting for WiFi)
    WiFi.disconnect(true, true); // disable wifi, erase ap info
    delay(1000);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pwd);
    WiFi.config(ip, gateway, subnet);
    while (WiFi.status() != WL_CONNECTED) { Serial.print("."); delay(500); }
    Serial.print("WiFi connected, IP = "); Serial.println(WiFi.localIP());


    // publish osc messages (default publish rate = 30 [Hz])

    OscWiFi.publish(host, publish_port, "/publish/value", i, f, s)
        ->setFrameRate(60.f);

    OscWiFi.publish(host, publish_port, "/publish/func", &millis, &micros)
        ->setIntervalMsec(500.f);


    // subscribe osc messages

    OscWiFi.subscribe(bind_port, "/bind/values", i, f, s);

    OscWiFi.subscribe(bind_port, "/lambda/bind/args",
        [&](const int& i, const float& f, const String& s)
        {
            Serial.print("/lambda/bind/args ");
            Serial.print(i); Serial.print(" ");
            Serial.print(f); Serial.print(" ");
            Serial.print(s); Serial.println();
        }
    );

    OscWiFi.subscribe(recv_port, "/lambda/msg",
        [](const OscMessage& m)
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

    OscWiFi.subscribe(recv_port, "/wildcard/*/test", [](const OscMessage& m)
    {
        Serial.print(m.remoteIP()); Serial.print(" ");
        Serial.print(m.remotePort()); Serial.print(" ");
        Serial.print(m.size()); Serial.print(" ");
        Serial.print(m.address()); Serial.print(" ");
        Serial.print(m.arg<int>(0)); Serial.println();
    });

    OscWiFi.subscribe(recv_port, "/need/reply", []()
    {
        Serial.println("/need/reply");

        int i = millis();
        float f = (float)micros() / 1000.f;
        String s = "hello";

        OscWiFi.send(host, send_port, "/reply", i, f, s);
    });

    OscWiFi.subscribe(recv_port, "/callback", onOscReceived);

}

void loop()
{
    OscWiFi.update(); // should be called to receive + send osc

    // or do that separately
    // OscWiFi.parse(); // to receive osc
    // OscWiFi.post(); // to publish osc
}
