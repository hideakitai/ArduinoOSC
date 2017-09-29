#include <SPI.h>
#include <Ethernet.h>

#include <ArdOSC.h>

byte myMac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte myIp[]  = { 192, 168, 0, 177 };
int  serverPort  = 10000;
int destPort=12000;
int ledPin =  9;

int flag=0;

OSCServer server;
OSCClient client;

void setup(){ 
  
 Serial.begin(19200);
 
 Ethernet.begin(myMac ,myIp); 
 server.begin(serverPort);
 
 //set callback function & oscaddress
 server.addCallback("/ard/aaa",&func1);
 
 pinMode(ledPin, OUTPUT);  
 
}
  
void loop(){
  
  if(server.aviableCheck()>0){
 //    Serial.println("alive! "); 
  }
  
}


void func1(OSCMessage *_mes){
  int value = _mes->getArgInt32(0);
  if(value!=1) return;
  
  //create new osc message
  OSCMessage newMes;
  
  //set destination ip address & port no
  newMes.setAddress(_mes->getIpAddress(),destPort);
  newMes.beginMessage("/ard/aaa");
  
  if(flag==1){
    flag=0;
    digitalWrite(ledPin, LOW);
  }
  else{
    flag=1;
    digitalWrite(ledPin, HIGH);
  }
  newMes.addArgInt32(flag);
  
  //send osc message
  client.send(&newMes);
  
}


