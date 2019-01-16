#include "ofMain.h"
#include "ofxOsc.h"

class ofApp : public ofBaseApp
{
	ofxOscSender sender;
	ofxOscReceiver receiver;
	
public:
	
	void setup()
	{
		ofSetBackgroundColor(0);
		
		sender.setup("192.168.1.201", 10000);
		receiver.setup(12000);
	}
	
	void update()
	{
		while(receiver.hasWaitingMessages())
		{
			// get the next message
			ofxOscMessage m;
			receiver.getNextMessage(m);
			
			cout << m.getAddress() << " " << m.getArgAsInt32(0) << " ";
			cout << m.getArgAsFloat(1) << " " << m.getArgAsDouble(2) << " ";
			cout << m.getArgAsString(3) << " " << m.getArgAsBool(4) << endl;
		}
	}
	
	void draw()
	{
		stringstream ss;
		ss << "press key to send osc" << endl;
		ss << "l: /lambda" << endl;
		ss << "c: /callback" << endl;
		ss << "w: /wildcard/abc/test" << endl;
		ss << "r: /need/reply" << endl;
		ofSetColor(255);
		ofDrawBitmapString(ss.str(), 20, 20);
	}
	
	void keyPressed(int key)
	{
		switch(key)
		{
			case 'l':
			{
				ofxOscMessage m;
				m.setAddress("/lambda");
				m.addInt32Arg(123);
				m.addFloatArg(4.5);
				m.addStringArg("six");
				sender.sendMessage(m, false);
				break;
			}
			case 'c':
			{
				ofxOscMessage m;
				m.setAddress("/callback");
				m.addInt32Arg(1);
				m.addFloatArg(2.2);
				m.addStringArg("test");
				sender.sendMessage(m, false);
				break;
			}
			case 'w':
			{
				ofxOscMessage m;
				m.setAddress("/wildcard/abc/test");
				m.addInt32Arg(1);
				sender.sendMessage(m, false);
				break;
			}
			case 'r':
			{
				ofxOscMessage m;
				m.setAddress("/need/reply");
				sender.sendMessage(m, false);
				break;
			}
		}
	}
};

//========================================================================
int main( ){
	ofSetupOpenGL(1024,768,OF_WINDOW);
	ofRunApp(new ofApp());
}
