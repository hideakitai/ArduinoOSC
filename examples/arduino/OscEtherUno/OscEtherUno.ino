// For Arduino Uno or such a small memory size boards,
// using manual send/parse are strongly recoomended.
// In such boards, the number of subscribers, publishers,
// and size of packet etc. are strictly restricted.
//
// If you really want to use subscriber and publisher,
// please adjust sizes depending on your applications.
//
// #define ARDUINOOSC_MAX_MSG_ARGUMENT_SIZE 8
// #define ARDUINOOSC_MAX_MSG_BYTE_SIZE 128
// #define ARDUINOOSC_MAX_MSG_QUEUE_SIZE 1
// #define ARDUINOOSC_MAX_PUBLISH_DESTINATION 4
// #define ARDUINOOSC_MAX_SUBSCRIBE_ADDRESS_PER_PORT 4
// #define ARDUINOOSC_MAX_SUBSCRIBE_PORTS 2

// #define ARDUINOOSC_DEBUGLOG_ENABLE
#include <ArduinoOSC.h>

// Ethernet stuff
uint8_t mac[] = {0xAB, 0xCD, 0xEF, 0x01, 0x23, 0x45};
const IPAddress ip(192, 168, 1, 201);
// Ethernet with useful options
// const IPAddress dns (192, 168, 1, 1);
// const IPAddress gateway (192, 168, 1, 1);
// const IPAddress subnet (255, 255, 255, 0);

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

// required to use manual packet parsing
OscEtherServer server(recv_port);
OscEtherClient client;
// OscEtherClient client(1234);  // you can set the local port of client manually (default: 9)

void setup() {
    Serial.begin(115200);
    delay(2000);

    // Ethernet stuff
    Ethernet.begin(mac, ip);

    // Ethernet with useful options
    // Ethernet.begin(mac, ip, dns, gateway, subnet); // full
    // Ethernet.setRetransmissionCount(4); // default: 8[times]
    // Ethernet.setRetransmissionTimeout(50); // default: 200[ms]

    Serial.println("start");
}

void loop() {
    // manual sending instead of publishers

    static uint32_t prev_value_ms = millis();
    if (millis() > prev_value_ms + 16) {
        i = millis();
        f = 1000000.f / (float)(micros() - prev_value_ms * 1000);
        s = String("testing");
        client.send(host, publish_port, F("/publish/value"), i, f, s);
        prev_value_ms = millis();
    }

    static uint32_t prev_func_ms = millis();
    if (millis() > prev_func_ms + 500) {
        client.send(host, publish_port, F("/publish/func"), millis(), micros());
        prev_func_ms = millis();
    }

    // manual parsing instead of subscribers

    if (server.parse()) {
        const OscMessage* msg = server.message();

        if (msg->address() == F("/lambda/msg")) {
            Serial.print(msg->remoteIP());
            Serial.print(" ");
            Serial.print(msg->remotePort());
            Serial.print(" ");
            Serial.print(msg->size());
            Serial.print(" ");
            Serial.print(msg->address());
            Serial.print(" ");
            Serial.print(msg->arg<int>(0));
            Serial.print(" ");
            Serial.print(msg->arg<float>(1));
            Serial.print(" ");
            Serial.print(msg->arg<String>(2));
            Serial.println();
        } else if (msg->address() == F("/wildcard/*/test")) {
            Serial.print(msg->remoteIP());
            Serial.print(" ");
            Serial.print(msg->remotePort());
            Serial.print(" ");
            Serial.print(msg->size());
            Serial.print(" ");
            Serial.print(msg->address());
            Serial.print(" ");
            Serial.print(msg->arg<int>(0));
            Serial.println();
        } else if (msg->address() == F("/need/reply")) {
            Serial.println(F("/need/reply"));
            int i = millis();
            float f = (float)micros() / 1000.f;
            String s = F("hello");
            client.send(host, send_port, F("/reply"), i, f, s);
        } else if (msg->address() == F("/callback")) {
            Serial.print(msg->remoteIP());
            Serial.print(" ");
            Serial.print(msg->remotePort());
            Serial.print(" ");
            Serial.print(msg->size());
            Serial.print(" ");
            Serial.print(msg->address());
            Serial.print(" ");
            Serial.print(msg->arg<int>(0));
            Serial.print(" ");
            Serial.print(msg->arg<float>(1));
            Serial.print(" ");
            Serial.print(msg->arg<String>(2));
            Serial.println();
        }
    }
}
