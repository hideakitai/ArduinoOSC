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
 
}

void draw() {

}




void oscEvent(OscMessage theOscMessage) {
  print("### received an osc message.");
 
  theOscMessage.print();
}
