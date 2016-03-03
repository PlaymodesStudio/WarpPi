#pragma once
#ifndef PM_WARPI_RENDERER_SCREEN
#define PM_WARPI_RENDERER_SCREEN

#include "pmWarpPiRenderer.h"
#include "ofMain.h"
#include "ofxHomography.h"

class pmWarpPiRendererScreen : public pmWarpPiRenderer
{
    
public:
    pmWarpPiRendererScreen();
    virtual void        update(ofEventArgs & a);
    virtual void        updateOSC(ofxOscMessage* m);
    virtual void        updateForScreen(){};
    virtual void        deleteRenderer();
    virtual void        draw();
    virtual void        onComplete(float* arg){};
    virtual void        saveConfigToXML();
    virtual void        loadConfigFromXML();
    virtual void        showScreenDebug();
    virtual void        showDebug(){};


    /// CLASS FUNCTIONS
    ////////////////////
    virtual void        setupScreen(ofVec2f _pos,ofVec2f _size);
    virtual void        drawIntoFbo(){};

    void keyPressed(ofKeyEventArgs &a);
    void keyReleased(ofKeyEventArgs &a);
    void mouseMoved(ofMouseEventArgs &a);
    void mouseDragged(ofMouseEventArgs &a);
    void mousePressed(ofMouseEventArgs &a);
    void mouseReleased(ofMouseEventArgs &a);

    void                testScreen();
    void                resetQuad();
//    void                showScreenDebug();
    
    /// CLASS PARAMS
    /////////////////
    ofFbo*              screenFbo;
    ofVec2f             screenPosition;
    ofVec2f             screenSize;
    
    ofImage*            testingImage;
    
    ofVec2f             screenDebugPosition;
    ofParameter<float>  screenOpacity;
    ofParameter<float>  maxScreenOpacity;
    

    /// HOMOGRAPHY
    ///////////////
    bool                doHomography;
    bool                doEditQuadPoints;
    int                 currentQuadPoint;
    ofPoint             originalCorners[4];
    ofPoint             distortedCorners[4];
    ofMatrix4x4*        homography;
    void                nextQuadPoint();
    void                previousQuadPoint();
    

    
};
#endif
