#pragma once

// OF
#include "ofMain.h"

// PM WARPI
#include "pmWarpPi.h"
#include "pmWarpPiRendererDmx.h"
#include "pmWarpPiRendererImagePlayer.h"
#ifdef TARGET_RASPBERRY_PI
    #include "pmWarpPiRendererOMXPlayer.h"
#endif 
#ifdef TARGET_OSX
    #include "pmWarpPiRendererVideoPlayer.h"
#endif


// OTHERS
#include<iterator>
#include<iostream>

//#define TARGET_OS_MAC


class ofApp : public ofBaseApp
{
public:

    // OF APP
    ///////////
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
    ////////////
    ofxOscReceiver              oscReceiverSTRING;
    ofxOscReceiver              oscReceiverOSC;
    
    ofxOscSender                oscSender;
    string                      lastOscMessage;
    ofxOscMessage*              processOSC(ofxOscMessage* m);

    /// TCP
    ////////////
    ofxTCPClient                tcpClient;
    bool                        tcpAreWeConnected;
    int                         tcpConnectTime;
    int                         tcpDeltaTime;
    string                      tcpMsgRx;
    ofxOscMessage*              processTCP(string s);

    /// WARPPI
    ////////////
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
    ///////////////
    vector<pmWarpPiRenderer*>    renderers;
    
    /// CONFIG
    ////////////
    void                        readConfig();
    int                         confId;
    int                         confOscReceivePort;
    int                         confOscReceivePortStringMode;
    int                         confOscSendPort;
    string                      confOscSendIpAddress;
    bool                        confHasVideo;
    bool                        confHasDmx;
    string                      confVideoFileName;
    string                      confName;
    int                         confDmxDevice;
    int                         confDmxNumChannels;
    int                         confDmxFirstChannel;
    bool                        confUsesTCP;
    string                      confTCPSendIpAddress;
    int                         confTCPPort;
    
    
};
