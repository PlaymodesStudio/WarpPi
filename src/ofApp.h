#pragma once

#include "ofMain.h"

#include "pmWarpPi.h"
#ifdef TARGET_RASPBERRY_PI
    #include "pmWarpPiRendererOMXPlayer.h"
#endif 
#ifdef TARGET_OS_MAC
    #include "pmWarpPiRendererVideoPlayer.h"
#endif

#include "pmWarpPiRendererDmx.h"

#include<iterator>
#include<iostream>

//#define TARGET_OS_MAC


class ofApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    
    void exit();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    /// OSC
    ofxOscReceiver              oscReceiverSTRING;
    ofxOscReceiver              oscReceiverOSC;
    
    ofxOscSender                oscSender;
    string                      lastOscMessage;
    ofxOscMessage*              processOSC(ofxOscMessage* m);

    /// WARPPI
    string                      name;
    string                      id;
    void                        showDebug();
    bool                        isDebugging;
    bool                        isTesting;
    ofVec2f                     debugPosition;
    bool                        useFbo;
    bool                        showFPS;
    
    void                        toggleDebug();
    void                        setDebug(bool b);
    void                        toggleTest();
    void                        setTest(bool b);
    
    /// RENDERERS
    vector<pmWarpPiRenderer*>    renderers;
    
    /// CONFIG
    void                        readConfig();
    int                         confId;
    int                         confOscReceivePort;
    int                         confOscReceivePortStringMode;
    int                         confOscSendPort;
    string                      confOscSendAddress;
    bool                        confHasVideo;
    bool                        confHasDmx;
    string                      confVideoFileName;
    string                      confName;
    int                         confDmxDevice;
    int                         confDmxNumChannels;
    int                         confDmxFirstChannel;
    
    
};
