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


void setup() {
    background(0);

  oscP5 = new OscP5(this,12000);
  
  
//  myRemoteLocation = new NetAddress("192.168.0.177",10000);
   myRemoteLocation = new NetAddress("224.0.0.0",10000);  //multicast address
  
  
}

void draw() {
  println("send1");
  sender1();
  delay(500);
  
  println("send2");
  sender2();
  delay(500);
  
  println("send3");
  sender3();
  delay(500);
  
}

void sender1(){
  OscMessage myMessage = new OscMessage("/ard/aaa");
  myMessage.add(1);
  oscP5.send(myMessage, myRemoteLocation);
}

void sender2(){
  OscMessage myMessage = new OscMessage("/ard/bbb");
  myMessage.add("send function2");
  oscP5.send(myMessage, myRemoteLocation);
}

void sender3(){
  OscMessage myMessage = new OscMessage("/ard/abcde");
  myMessage.add("send function3");
  oscP5.send(myMessage, myRemoteLocation);
}



void oscEvent(OscMessage theOscMessage) {
  print("### received an osc message.");
 
  theOscMessage.print();
}
