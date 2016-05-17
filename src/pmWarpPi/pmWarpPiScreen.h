//
//  pmWarpPiScreen.h
//  WarpPi
//
//  Created by Eduard Frigola on 12/05/16.
//
//

#ifndef pmWarpPiScreen_h
#define pmWarpPiScreen_h

#include "pmWarpPi.h"
#include "pmWarpPiRendererDrawable.h"
#include "pmWarpPiRendererVideoPlayer.h"
#include "pmWarpPiRendererImagePlayer.h"
#ifdef TARGET_RASPBERRY_PI
    #include "pmWarpPiRendererOMXPlayer.h"
#endif
#include "ofxHomography.h"

class pmWarpPiScreen
{
    
public:
    
    /// CLASS FUNCTIONS
    ////////////////////
    pmWarpPiScreen();
    
    void        setup(bool hasVideo, bool hasImage, bool _useFbo, bool _useHomograhy);
    void        update();
    void        updateOSC(ofxOscMessage* m);
    void        draw();
    void        saveConfigToXML();
    void        loadConfigFromXML();
    void        showDebug();
    void        setIsTesting(bool t){isTesting=t;};
    void        setIsDebugging(bool b){isDebugging=b;};
    
    void        swapToImage(float &f);
    void        swapToVideo(float &f);
    
    void        keyPressed(ofKeyEventArgs &a);
    void        keyReleased(ofKeyEventArgs &a){};
    void        mouseMoved(ofMouseEventArgs & args){};
    void        mouseDragged(ofMouseEventArgs & args){};
    void        mousePressed(ofMouseEventArgs & args){};
    void        mouseReleased(ofMouseEventArgs & args){};
    void        mouseScrolled(ofMouseEventArgs & args){};
    void        mouseEntered(ofMouseEventArgs & args){};
    void        mouseExited(ofMouseEventArgs & args){};
    
    
    /// screen PARAMS
    /////////////////
    bool                isDebugging;
    bool                isTesting;
    
    void                resetQuad();
    
    /// CLASS PARAMS
    /////////////////
    
    ofRectangle         screenRect;
    ofRectangle         elementRect;
    ofRectangle         elementDebugRect;
    
    ofFbo*              screenFbo;
    ofVec2f             screenPosition;
    ofVec2f             screenSize;
    
    ofImage*            testingImage;
    
    ofVec2f             screenDebugPosition;
    
    
    
    /// HOMOGRAPHY
    ///////////////
    bool                doHomography;
    bool                useFbo;
    bool                doEditQuadPoints;
    int                 currentQuadPoint;
    ofPoint             originalCorners[4];
    ofPoint             distortedCorners[4];
    ofMatrix4x4*        homography;
    void                nextQuadPoint();
    void                previousQuadPoint();
    
    vector<pmWarpPiRendererDrawable*> screenRenderers;
    
};



#endif /* pmWarpPiScreen_h */
