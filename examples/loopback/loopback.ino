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
