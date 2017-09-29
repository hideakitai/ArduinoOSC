#include <SPI.h>
#include <Ethernet.h>

#include <ArdOSC.h>

byte myMac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte myIp[]  = { 192, 168, 0, 177 };
int  serverPort  = 10000;

int ledPin =  9;

int t=100;

OSCServer server;

void setup(){ 
  
 Serial.begin(19200);
 
 Ethernet.begin(myMac ,myIp); 
 server.begin(serverPort);
 
 //set callback function
 server.addCallback("/ard/aaa",&func1);
 
 pinMode(ledPin, OUTPUT);  
 
}
  
void loop(){
  if(server.aviableCheck()>0){
 //    Serial.println("alive! "); 
  }
  
  digitalWrite(ledPin, HIGH);
  delay(50);
  digitalWrite(ledPin, LOW);
  delay(t);
}


void func1(OSCMessage *_mes){

  //get 1st argument(int32)
  t = _mes->getArgInt32(0);



}


