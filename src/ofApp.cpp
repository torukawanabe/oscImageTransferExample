#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    settings.load(SETTING_FILE);
    settings.setToParent();
    
    ofBackground(255, 192, 203);
    
    sender.setup(settings.getValue("imageTransfer/client/address"), PORT);
    receiver.setup(PORT);
    
}

//--------------------------------------------------------------
void ofApp::update(){
    updateOSC();
}

void ofApp::updateOSC(){
    while(receiver.hasWaitingMessages()){
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage(m);
        if(m.getAddress() == "/image" ){
            bool isFirst = m.getArgAsBool(0);
            bool isLast = m.getArgAsBool(1);
            
            if(isFirst){
                receivedBuffer.clear();
            }
            
            ofBuffer buff = m.getArgAsBlob(2);
            for (char c: buff){
                receivedBuffer.push_back(c);
            }
            
            // ofBufferが最後に0を挿入するので末尾を削除
            receivedBuffer.pop_back();
            
            if(isLast){
                ofBuffer imgBuff = ofBuffer(receivedBuffer.data(), receivedBuffer.size());
                receivedImg.load(imgBuff);
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    if(img.isAllocated()){
        ofDrawBitmapString("Image:", 10, 160);
        float width = 490;
        float ratio = width / img.getWidth();
        img.draw(10, 180, width, img.getHeight()*ratio);
    }
    
    if(receivedImg.isAllocated()){
        ofDrawBitmapString("Recived Image:", 510, 160);
        float width = 490;
        float ratio = width / receivedImg.getWidth();
        receivedImg.draw(510, 180, width, receivedImg.getHeight()*ratio);
    }
    
    // display instructions
    string buf;
    buf = "sending osc messages to " + string(settings.getValue("imageTransfer/client/address"))  + ":" + ofToString(PORT);
    ofDrawBitmapString(buf, 10, 20);
    ofDrawBitmapString("drag image file for sending via OSC", 10, 50);

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    if (dragInfo.files.size() > 0) {
        string ext = ofFilePath::getFileExt(dragInfo.files[0]);
        if(ext == "png" || ext == "PNG" ||  ext == "jpg" || ext == "jpeg" || ext == "JPG" || ext == "JPEG"){
            imgAsBuffer = ofBufferFromFile(dragInfo.files[0]);
            img.load(imgAsBuffer);
            
            sendImage();
        }
    }
}


void ofApp::sendImage(){
    img.load(imgAsBuffer);
    
    static const int dividingSize = 60000;
    int numOfSend = (imgAsBuffer.size() / dividingSize) + 1;
    cout << "NOW POST IMG SIZE:" << imgAsBuffer.size() << " ,NUM OF SEND:" << numOfSend << endl;
    
    if (numOfSend == 1) {
        ofxOscMessage m;
        m.setAddress("/image");
        m.addBoolArg(true);
        m.addBoolArg(true);
        
        vector<char> sendBuff = vector<char>(imgAsBuffer.begin(), imgAsBuffer.end());
        m.addBlobArg(ofBuffer(sendBuff.data(), sendBuff.size()));
        
        sender.sendMessage(m, false);
    }else{
        vector<char>::iterator it = imgAsBuffer.begin();
        for (int i=0; i<numOfSend; i++) {
            ofxOscMessage m;
            
            vector<char> sendBuff;
            sendBuff.clear();
            
            m.setAddress("/image");
            if(i == 0){
                m.addBoolArg(true);
                m.addBoolArg(false);
            }else if(i == (numOfSend - 1)){
                m.addBoolArg(false);
                m.addBoolArg(true);
            }else{
                m.addBoolArg(false);
                m.addBoolArg(false);
            }
            
            
            for(int j=0; j<dividingSize; j++){
                sendBuff.push_back(*it);
                if(it == imgAsBuffer.end()) break;
                it++;
            }
            cout << "sendBuff" << i << ":" << sendBuff.size() << endl;
            m.addBlobArg(ofBuffer(sendBuff.data(), sendBuff.size()));
            sender.sendMessage(m, false);
        }
        
    }
    
    cout << "ofApp:: sending image with size: " << imgAsBuffer.size() << endl;
}

