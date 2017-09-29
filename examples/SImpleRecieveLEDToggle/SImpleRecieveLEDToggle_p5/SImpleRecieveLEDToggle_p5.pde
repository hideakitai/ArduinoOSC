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
  
  
}
void mousePressed(){
   println("send!!");
    sender();
}

void sender(){
  OscMessage myMessage = new OscMessage("/ard/aaa");
  myMessage.add(1);
  oscP5.send(myMessage, myRemoteLocation);
}



void oscEvent(OscMessage theOscMessage) {
  print("### received an osc message.");
 
  theOscMessage.print();
  if(theOscMessage.checkAddrPattern("/ard/aaa")==true) {
    if(theOscMessage.checkTypetag("i")) {
      int firstValue = theOscMessage.get(0).intValue();
      if(firstValue==1) background(100,100,0);
      else background(0,0,0);
    }
  }
}
