#pragma once
#ifndef PM_WARPI_RENDERER_VIDEOPLAYER
#define PM_WARPI_RENDERER_VIDEOPLAYER

#include "pmWarpPiRendererScreen.h"
#include "ofMain.h"

#ifdef TARGET_OS_MAC


class pmWarpPiRendererVideoPlayer : public pmWarpPiRendererScreen
{
    
public:
 
    pmWarpPiRendererVideoPlayer();
    virtual void        updateOSC(ofxOscMessage* m);
    virtual void        deleteRenderer();

    /// CLASS FUNCTIONS
    ////////////////////
    virtual void        updateForScreen();

    virtual void        drawIntoFbo();
    virtual void        setupVideoPlayer(string _name,ofVec2f _pos, ofVec2f _size);
    virtual void        onComplete(float* arg);
    virtual void        showDebug();
//    void                showVideoPlayerDebug();
    
    void keyPressed(ofKeyEventArgs &a);
    void keyReleased(ofKeyEventArgs &a);
    void mouseMoved(ofMouseEventArgs &a);
    void mouseDragged(ofMouseEventArgs &a);
    void mousePressed(ofMouseEventArgs &a);
    void mouseReleased(ofMouseEventArgs &a);

    /// CLASS PARAMS
    /////////////////
    ofVideoPlayer*      videoPlayer;
    string              videoFileName;
    ofVec2f             videoPosition;
    ofVec2f             videoSize;
    ofVec2f             videoPlayerDebugPosition;
    
    
    
};
#endif
#endif