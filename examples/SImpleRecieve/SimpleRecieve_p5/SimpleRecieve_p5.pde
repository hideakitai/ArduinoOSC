/**
 * Mouse Functions. 
 * 
 * Click on the box and drag it across the screen. 
 */
 
import oscP5.*;
import netP5.*;

OscP5 oscP5;
NetAddress myRemoteLocation;

int bx;
int by;
int bs = 20;
boolean bover = false;
boolean locked = false;
int bdifx = 0; 
int bdify = 0; 
int oldv;

void setup() 
{
  size(200, 400);
  bx = width/2;
  by = height/2;
  rectMode(RADIUS); 
 
 oscP5 = new OscP5(this,12000);
 myRemoteLocation = new NetAddress("224.0.0.0",10000);
 
}

void draw() 
{ 
  background(0);
  
  // Test if the cursor is over the box 
  if (mouseX > bx-bs && mouseX < bx+bs && 
      mouseY > by-bs && mouseY < by+bs) {
    bover = true;  
    if(!locked) { 
      stroke(255); 
      fill(153);
    } 
  } else {
    stroke(153);
    fill(153);
    bover = false;
  }
  
  // Draw the box
  rect(bx, by, bs, bs);
}

void mousePressed() {
  if(bover) { 
    locked = true; 
    fill(255, 255, 255);
  } else {
    locked = false;
  }
//  bdifx = mouseX-bx; 
  bdify = mouseY-by; 

}

void mouseDragged() {
  if(locked) {
  if(mouseY>360)  by = 360;
  else if(mouseY<30)  by = 60;
  else by = mouseY-bdify;
  print(by+"\n");
  
  int value=by;
  value = int(value/100)*100;
  if(oldv==value) return;
  OscMessage myMessage = new OscMessage("/ard/aaa");
  
  myMessage.add(value); 
  oscP5.send(myMessage, myRemoteLocation);
  
  oldv=value;
  }
  
}

void mouseReleased() {
  locked = false;
}

