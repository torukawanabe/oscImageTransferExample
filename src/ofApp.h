#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

#define SETTING_FILE "settings.xml"
#define PORT 12345

//--------------------------------------------------------
class ofApp : public ofBaseApp {
    
public:
    
    void setup();
    void update();
    void draw();
    
    void dragEvent(ofDragInfo dragInfo);

    void updateOSC();
    void sendImage();
    
    ofXml settings;

    ofTrueTypeFont font;
    ofxOscSender sender;
    ofxOscReceiver receiver;

    ofBuffer imgAsBuffer;
    vector<char> receivedBuffer;
    ofImage img, receivedImg;
};

