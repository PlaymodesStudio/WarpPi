#pragma once
#ifndef PM_WARPI_RENDERER_DMX
#define PM_WARPI_RENDERER_DMX

#include "pmWarpPiRenderer.h"
#include "ofMain.h"
#include "ofxDmx.h"

class pmWarpPiRendererDmx : public pmWarpPiRenderer
{
    
public:
 
    pmWarpPiRendererDmx();
    
    virtual void        update(ofEventArgs & a);
    virtual void        updateOSC(ofxOscMessage* m);
    virtual void        draw();
    virtual void        deleteRenderer();
    virtual void        updateForScreen(){};
    virtual void        setIsTesting(bool b);
    virtual void        setIsDebugging(bool b);


    /// CLASS FUNCTIONS
    ////////////////////
    void                setupDmx(int _numDevice,int _numChannels,int _firstChannel);
    virtual void        onComplete(float* arg);
    void                testDmx();
    void                showDmxDebug();
    void                setChannel(int _ch,int _v);
    void                setAllChannelsTo(int v);

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
    ofVec2f                         dmxDebugPosition;
    ofxDmx*                         dmx;
    int                             numChannels;
    int                             firstChannel;
    int                             numDevice;
    vector < ofParameter <float> >  channels;
    float                           testValue;
//    ofParameter <float>   channels;
    
};
#endif
