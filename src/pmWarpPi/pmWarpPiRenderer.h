#pragma once
#ifndef PM_WARPI_RENDERER
#define PM_WARPI_RENDERER
#endif

//#include "ofMain.h"
//#include "ofxGui.h"
//#include "ofxOsc.h"
//#include "ofxTweenzor.h"

#include "pmWarpPi.h"

/* FORWARD DECLARATION !!
//#include "pmLedsC4.h"
class pmLedsC4Screen;
*/

enum renderType{
    renderImage,
    renderVideo,
    renderDmx,
    renderArtNet
};

class pmWarpPiRenderer
{
    
public:
    
    /// CLASS FUNCTIONS
    ////////////////////
    pmWarpPiRenderer();
    
    virtual void        setup(string id);
    virtual void        update(ofEventArgs & a);
    virtual void        updateOSC(ofxOscMessage* m);
    virtual void        draw() {};
    virtual void        deleteRenderer();
    virtual void        onComplete(float* arg){};
    virtual void        updateForScreen(){};
    virtual void        saveConfigToXML(){};
    virtual void        loadConfigFromXML(){};
    virtual void        showDebug(){};
    virtual void        setIsTesting(bool b){isTesting=b;};
    virtual void        setIsDebugging(bool b){isDebugging=b;};
    renderType          getType(){return type;};

    

    /// CLASS PARAMS
    /////////////////
    bool                isDebugging;   
    bool                isTesting;
    string              id;
    renderType          type;
    
    /// OSC
    ////////
    ofxOscSender*       oscSender;
    string              oscSenderAddress;
    int                 oscSenderPort;
    
    
    /// OFXGUI
    //////////
    ofxPanel*            gui;
    bool                 guiIsSetup;

};
