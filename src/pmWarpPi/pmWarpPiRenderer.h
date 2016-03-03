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


class pmWarpPiRenderer
{
    
public:
    
    /// CLASS FUNCTIONS
    ////////////////////
    pmWarpPiRenderer();
    
    virtual void        setup(string id, string _oscSendAddress, int _oscSendPort);
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

    void mouseMoved(ofMouseEventArgs &a){};
    void mouseDragged(ofMouseEventArgs &a){};
    void mousePressed(ofMouseEventArgs &a){};
    void mouseReleased(ofMouseEventArgs &a){};

    

    /// CLASS PARAMS
    /////////////////
    bool                isDebugging;   
    bool                isTesting;
    string              id;
    
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
