/**
 * oscP5oscArgument by andreas schlegel
 * example shows how to parse incoming osc messages "by hand".
 * it is recommended to take a look at oscP5plug for an alternative way to parse messages.
 * oscP5 website at http://www.sojamo.de/oscP5
 */

import oscP5.*;
import netP5.*;

OscP5 oscP5;
NetAddress myRemoteLocation;

int a;
float b;

void setup() {
    background(0);

  oscP5 = new OscP5(this,12000);
  
  
//  myRemoteLocation = new NetAddress("192.168.0.177",10000);
   myRemoteLocation = new NetAddress("224.0.0.0",10000);  //multicast address
  
 
 a=0;
 b=0.0;
  
}

void draw() {
   
   sender();
  
  delay(1000);
  
}

void sender(){
  a+=1;
  b+=0.1;
  if(a>999) a=0;
  if(b>999.0) b=0.0;
  
  OscMessage myMessage = new OscMessage("/ard/aaa");
  
  myMessage.add(a); 
  myMessage.add(b); 
  myMessage.add("some text");
  oscP5.send(myMessage, myRemoteLocation);
}



void oscEvent(OscMessage theOscMessage) {
  print("### received an osc message.");
 
  theOscMessage.print();
}
