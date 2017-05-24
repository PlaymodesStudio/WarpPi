//
//  pmWarpPiRendererDmx.h
//  WarpPi
//
//  Created by Eduard Frigola on 04/05/16.
//
//

#ifndef pmWarpPiRendererDmx_h
#define pmWarpPiRendererDmx_h

#include "pmWarpPi.h"
#include "pmWarpPiRendererVideoPlayer.h"
#include "ofxDmx.h"


class pmWarpPiRendererDmx: public pmWarpPiRendererVideoPlayer{
    
public:
    pmWarpPiRendererDmx();
    
    virtual void        updateOSC(ofxOscMessage* m); //implementar i que overridii els de videoplyer i cridi els play, no els parseji directe.
    virtual void        deleteRenderer(){};
    
    /// CLASS FUNCTIONS
    ////////////////////
    virtual void        updateForScreen();
    void                setupDmx(string _name, int _numDevice = 0, bool _active = true);
    virtual void        showDebug();

    void start();
    void setFromPixels(ofPixels &pixels);
    bool sendDmx();
    bool sendDmx(ofPixels &pixels);
    bool isStarted(){return bStarted;};
    
private:
    bool bStarted;
    ofxDmx*                         dmx;
    int numDevice;
    vector<unsigned char> linepixels;
    
};


#endif /* pmWarpPiRendererDmx_h */
