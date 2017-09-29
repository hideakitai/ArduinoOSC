#include <SPI.h>
#include <Ethernet.h>

#include <ArdOSC.h>

byte myMac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte myIp[]  = { 192, 168, 0, 177 };
int  serverPort  = 10000;

int destPort=12000;
  
OSCServer server;
OSCClient client;

int v1=0;
int v2=10;
int v3=20;

void setup(){ 
  
 Serial.begin(19200);
 
 Ethernet.begin(myMac ,myIp); 
 server.begin(serverPort);
 
 //set callback function
 server.addCallback("/ard/aaa",&func1);
 server.addCallback("/ard/bbb",&func2);
 server.addCallback("/ard/abcde",&func3);
}
  
void loop(){
  
  int result = server.aviableCheck();
  
  if(result>0){
    //callback after process
     Serial.println("alive! "); 
  }
  
}


void func1(OSCMessage *_mes){
//  logMes(_mes);
  
  //create new osc message
  OSCMessage newMes;
  
  //set destination ip address & port no
  newMes.setAddress(_mes->getIpAddress(),destPort);
  
  if(v1++>1000) v1=0;
  //set argument
  newMes.beginMessage("/ard/aaa");
  newMes.addArgInt32(v1);
  newMes.addArgString("function1!");
  //send osc message
  client.send(&newMes);
 
}

void func2(OSCMessage *_mes){
 //  logMes(_mes);
  
  //create new osc message
  OSCMessage newMes;
  
  //set destination ip address & port no
  newMes.setAddress(_mes->getIpAddress(),destPort);
  
  if(v2++>1000) v2=0;
  
  //set argument
  newMes.beginMessage("/ard/bbb");
  newMes.addArgInt32(v2);
  newMes.addArgString("function2!");

  //send osc message
  client.send(&newMes);
}

void func3(OSCMessage *_mes){
 // logMes(_mes);

  //create new osc message
  OSCMessage newMes;
  
  //set destination ip address & port no
  newMes.setAddress(_mes->getIpAddress(),destPort);
  
   if(v3++>1000) v3=0;
   
  //set argument
  newMes.beginMessage("/ard/abcde");
  newMes.addArgInt32(v3);
  newMes.addArgString("function3!");

  //send osc message
  client.send(&newMes);

}


void logMes(OSCMessage *_mes){
  logIp(_mes);
  logOscAddress(_mes);
  
  //get source ip address
  byte *sourceIp = _mes->getIpAddress();

  //get 1st argument(string)
  int strSize=_mes->getArgStringSize(0);
  char tmpStr[strSize]; //string memory allocation
  _mes->getArgString(0,tmpStr); 
  
  Serial.println(tmpStr);
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

