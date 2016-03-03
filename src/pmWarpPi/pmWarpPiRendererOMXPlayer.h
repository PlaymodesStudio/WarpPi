#pragma once
#ifndef PM_WARPI_RENDERER_OMXPLAYER
#define PM_WARPI_RENDERER_OMXPLAYER

#ifdef TARGET_RASPBERRY_PI

#include "ofxOMXPlayer.h"
#include "pmWarpPiRendererScreen.h"
#include "ofMain.h"

class pmWarpPiRendererOMXPlayer : public pmWarpPiRendererScreen
{
    
public:
 
    pmWarpPiRendererOMXPlayer();
    
    virtual void        updateOSC(ofxOscMessage* m);
    
    virtual void        deleteRenderer();
    virtual void        showDebug();
    
    /// CLASS FUNCTIONS
    ////////////////////
    virtual void        updateForScreen();

    virtual void        drawIntoFbo();
    virtual void        setupOMXPlayer(string _name,ofVec2f _pos, ofVec2f _size);
    virtual void        onComplete(float* arg);
//    void                showVideoPlayerDebug();
    void                testVideo(bool test);
    
    /// CLASS PARAMS
    /////////////////
    ofxOMXPlayer*       omxPlayer;
    //~ ofxOMXPlayer	testPlayer;
    string              videoFileName;
    ofVec2f             videoPosition;
    ofVec2f             videoSize;
    ofVec2f             videoPlayerDebugPosition;
    
    
    
};
#endif
#endif
