#include <ArduinoOSC.h>

WiFiUDP udp;
ArduinoOSCWiFi osc;
const char* ssid = "yout_ssid";
const char* pwd = "your_password";
const IPAddress ip(192, 168, 1, 201);
const IPAddress gateway(192, 168, 1, 1);
const IPAddress subnet(255, 255, 255, 0);
const char* host = "192.168.1.200";
const int recv_port = 10000;
const int send_port = 12000;

void setup()
{
    Serial.begin(115200);
    WiFi.disconnect(true);
    WiFi.begin(ssid, pwd);
    WiFi.config(ip, gateway, subnet);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    osc.begin(udp, recv_port);
    osc.addCallback("/ard/aaa", &callback);
    osc.addCallback("/ard", &callback);
}

void loop()
{
    osc.parse();
}

void callback(OSCMessage& m)
{
    //create new osc message
    OSCMessage msg;
    //set destination ip address & port no
    msg.beginMessage(host, send_port);
    //set argument
    msg.setOSCAddress(m.getOSCAddress());
    msg.addArgInt32(m.getArgAsInt32(0));
    msg.addArgFloat(m.getArgAsFloat(1));
    msg.addArgString(m.getArgAsString(2));
    //send osc message
    osc.send(msg);
}
