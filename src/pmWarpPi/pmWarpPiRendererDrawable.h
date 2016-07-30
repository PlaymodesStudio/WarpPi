#pragma once
#ifndef PM_WARPI_RENDERER_SCREEN
#define PM_WARPI_RENDERER_SCREEN

#include "pmWarpPiRenderer.h"

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


    /// CLASS FUNCTIONS
    ////////////////////
    virtual void        setupScreen();
    virtual void        drawElement(ofRectangle container){};
    
    ofEvent<float>      swapEvent;
    
    /// CLASS PARAMS
    /////////////////
    
    ofRectangle         screenRect;
    ofRectangle         elementRect;
    ofRectangle         elementDebugRect;
    ofPoint             elementDebugInfoPos;
    
    ofFbo*              screenFbo;
    ofVec2f             screenPosition;
    ofVec2f             screenSize;
    
    ofVec2f             screenDebugPosition;
    ofParameter<float>  screenOpacity;
    ofParameter<float>  maxScreenOpacity;
    
    bool                activePlayer;
    bool                isFading;
    float               audioVolume;

    

    
};
#endif
