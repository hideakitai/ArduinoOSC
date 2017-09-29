#include <SPI.h>
#include <Ethernet.h>

#include <ArdOSC.h>

byte myMac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte myIp[]  = { 192, 168, 0, 177 };
int  serverPort  = 10000;

int destPort=12000;

char oscadr[]="/ard/aaa";

OSCServer server;
OSCClient client;


void setup(){ 
  
// Serial.begin(19200);
 
 Ethernet.begin(myMac ,myIp); 
 server.begin(serverPort);
 
 //set callback function
 server.addCallback(oscadr,&func1);
 
}
  
void loop(){
  if(server.aviableCheck()>0){
//     Serial.println("alive! "); 
  }
}


void func1(OSCMessage *_mes){
  
  logIp(_mes);
  logOscAddress(_mes);
  
  //get source ip address
  byte *sourceIp = _mes->getIpAddress();

  //get 1st argument(int32)
  int tmpI=_mes->getArgInt32(0);
  
  //get 2nd argument(float)
  float tmpF=_mes->getArgFloat(1);
  
  //get 3rd argument(string)
  int strSize=_mes->getArgStringSize(2);
  char tmpStr[strSize]; //string memory allocation
  _mes->getArgString(2,tmpStr); 



  //create new osc message
  OSCMessage newMes;
  
  //set destination ip address & port no
  newMes.setAddress(sourceIp,destPort);
  
  //set argument
  newMes.beginMessage(oscadr);
  newMes.addArgInt32(tmpI+1);
  newMes.addArgFloat(tmpF+0.1);
  newMes.addArgString(tmpStr);
 
  //send osc message
  client.send(&newMes);

}




void logIp(OSCMessage *_mes){
  byte *ip = _mes->getIpAddress();
  Serial.print("IP:");
  Serial.print(ip[0],DEC);
  Serial.print(".");
  Serial.print(ip[1],DEC);
  Serial.print(".");
  Serial.print(ip[2],DEC);
  Serial.print(".");
  Serial.print(ip[3],DEC);
  Serial.print(" ");
}

void logOscAddress(OSCMessage *_mes){
  Serial.println(_mes->getOSCAddress());
} 

