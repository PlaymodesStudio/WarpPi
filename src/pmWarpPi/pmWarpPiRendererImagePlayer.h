#pragma once
#ifndef PM_WARPI_RENDERER_IMAGEPLAYER
#define PM_WARPI_RENDERER_IMAGEPLAYER

#include "pmWarpPiRendererScreen.h"
#include "ofMain.h"



class pmWarpPiRendererImagePlayer : public pmWarpPiRendererScreen
{
    
public:
 
    pmWarpPiRendererImagePlayer();
    virtual void        updateOSC(ofxOscMessage* m);
    virtual void        deleteRenderer();

    /// CLASS FUNCTIONS
    ////////////////////
    virtual void        updateForScreen();

    virtual void        drawIntoFbo();
    virtual void        setupImagePlayer(string _name,ofVec2f _pos, ofVec2f _size);
    virtual void        onComplete(float* arg);
    virtual void        showDebug();
//    void                showVideoPlayerDebug();
    
    void keyPressed(ofKeyEventArgs &a);
    void keyReleased(ofKeyEventArgs &a);
//    void mouseMoved(ofMouseEventArgs &a);
//    void mouseDragged(ofMouseEventArgs &a);
//    void mousePressed(ofMouseEventArgs &a);
//    void mouseReleased(ofMouseEventArgs &a);
    void mouseMoved(ofMouseEventArgs & args);
    void mouseDragged(ofMouseEventArgs & args);
    void mousePressed(ofMouseEventArgs & args);
    void mouseReleased(ofMouseEventArgs & args);
    void mouseScrolled(ofMouseEventArgs & args){};
    void mouseEntered(ofMouseEventArgs & args){};
    void mouseExited(ofMouseEventArgs & args){};

    /// CLASS PARAMS
    /////////////////
    ofImage*            imagePlayer;
//    ofVideoPlayer*      videoPlayer;
    string              videoFileName;
    ofVec2f             videoPosition;
    ofVec2f             videoSize;
    ofVec2f             videoPlayerDebugPosition;
    
    
    
};
#endif