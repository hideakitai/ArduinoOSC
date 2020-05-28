#include "ofMain.h"
#include "ofxOsc.h"

class ofApp : public ofBaseApp
{
    ofxOscSender sender_send;
    ofxOscSender sender_bind;
    ofxOscReceiver receiver_recv;
    ofxOscReceiver receiver_published;
    std::string recv_reply;
    std::string recv_pub_value;
    std::string recv_pub_func;

public:

    void setup()
    {
        ofSetBackgroundColor(0);

        sender_send.setup("192.168.1.201", 54321);
        receiver_recv.setup(55555);

        sender_bind.setup("192.168.1.201", 54345);
        receiver_published.setup(54445);
    }

    void update()
    {
        while(receiver_recv.hasWaitingMessages())
        {
            // get the next message
            ofxOscMessage m;
            receiver_recv.getNextMessage(m);

            if (m.getAddress() == "/reply")
            {
                recv_reply = "";
                recv_reply += m.getAddress() + " ";
                recv_reply += ofToString(m.getArgAsInt32(0)) + " ";
                recv_reply += ofToString(m.getArgAsFloat(1)) + " ";
                recv_reply += m.getArgAsString(2) + "\n";
            }
        }

        while(receiver_published.hasWaitingMessages())
        {
            ofxOscMessage m;
            receiver_published.getNextMessage(m);

            if (m.getAddress() == "/publish/value")
            {
                recv_pub_value = "";
                recv_pub_value += m.getAddress() + " ";
                recv_pub_value += ofToString(m.getArgAsInt32(0)) + " ";
                recv_pub_value += ofToString(m.getArgAsFloat(1)) + " ";
                recv_pub_value += m.getArgAsString(2) + "\n";
            }
            else if (m.getAddress() == "/publish/func")
            {
                recv_pub_func = "";
                recv_pub_func += m.getAddress() + " ";
                recv_pub_func += ofToString(m.getArgAsInt32(0)) + " ";
                recv_pub_func += ofToString(m.getArgAsInt32(1)) + "\n";
            }
        }
    }

    void draw()
    {
        stringstream ss;
        ss << "press key to send osc" << endl << endl;
        ss << "l: /lambda/msg" << endl;
        ss << "c: /callback" << endl;
        ss << "w: /wildcard/abc/test" << endl;
        ss << "r: /need/reply" << endl;
        ss << "b: /bind/values" << endl;
        ss << "a: /lambda/bind/args" << endl;
        ofSetColor(255);
        ofDrawBitmapString(ss.str(), 20, 20);
        ofDrawBitmapString(recv_reply, 20, 160);
        ofDrawBitmapString(recv_pub_value, 20, 200);
        ofDrawBitmapString(recv_pub_func, 20, 240);
    }

    void keyPressed(int key)
    {
        switch(key)
        {
            case 'l':
            {
                ofxOscMessage m;
                m.setAddress("/lambda/msg");
                m.addInt32Arg(123);
                m.addFloatArg(4.5);
                m.addStringArg("six");
                sender_send.sendMessage(m, false);
                break;
            }
            case 'c':
            {
                ofxOscMessage m;
                m.setAddress("/callback");
                m.addInt32Arg(1);
                m.addFloatArg(2.2);
                m.addStringArg("test");
                sender_send.sendMessage(m, false);
                break;
            }
            case 'w':
            {
                ofxOscMessage m;
                m.setAddress("/wildcard/abc/test");
                m.addInt32Arg(1);
                sender_send.sendMessage(m, false);
                break;
            }
            case 'r':
            {
                ofxOscMessage m;
                m.setAddress("/need/reply");
                sender_send.sendMessage(m, false);
                break;
            }
            case 'b':
            {
                ofxOscMessage m;
                m.setAddress("/bind/values");
                m.addInt32Arg(ofGetFrameNum());
                m.addFloatArg(ofGetFrameRate());
                m.addStringArg("string");
                sender_bind.sendMessage(m, false);
                break;
            }
            case 'a':
            {
                ofxOscMessage m;
                m.setAddress("/lambda/bind/args");
                m.addInt32Arg(ofGetFrameNum());
                m.addFloatArg(ofGetFrameRate());
                m.addStringArg("string");
                sender_bind.sendMessage(m, false);
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
