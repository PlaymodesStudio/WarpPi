#pragma once
#ifndef PM_WARPI_RENDERER_SCREEN
#define PM_WARPI_RENDERER_SCREEN

#include "pmWarpPiRenderer.h"
#include "ofMain.h"
#include "ofxHomography.h"

class pmWarpPiRendererDrawable : public pmWarpPiRenderer
{
    
public:
    pmWarpPiRendererDrawable();
    virtual void        update(ofEventArgs & a);
    virtual void        updateOSC(ofxOscMessage* m);
    virtual void        updateForScreen(){};
    virtual void        deleteRenderer();
    virtual void        draw();
    virtual void        onComplete(float* arg){};

    virtual void        showScreenDebug();
    virtual void        showDebug(){};
    virtual void        fadeIn(float fadeTime);
    virtual void        fadeOut(float fadeTime);
    virtual void        onFadeComplete(float* args);


    /// CLASS FUNCTIONS
    ////////////////////
    virtual void        setupScreen();
    virtual void        drawElement(ofRectangle container){};

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
    
    ofEvent<float>      swapEvent;
    
//    void                showScreenDebug();
    
    /// CLASS PARAMS
    /////////////////
    
    ofRectangle         screenRect;
    ofRectangle         elementRect;
    ofRectangle         elementDebugRect;
    ofPoint             elementDebugInfoPos;
    
    ofFbo*              screenFbo;
    ofVec2f             screenPosition;
    ofVec2f             screenSize;
    
    ofImage*            testingImage;
    
    ofVec2f             screenDebugPosition;
    ofParameter<float>  screenOpacity;
    ofParameter<float>  maxScreenOpacity;
    
    bool                activePlayer;

    

    
};
#endif
