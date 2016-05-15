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

class pmWarpPiScreen
{
    
public:
    
    /// CLASS FUNCTIONS
    ////////////////////
    pmWarpPiScreen();
    
    void        setup();
    void        update();
    void        updateOSC(ofxOscMessage* m);
    void        draw();
    void        saveConfigToXML(){};
    void        loadConfigFromXML(){};
    void        showDebug(){};
    void        setIsTesting(bool t){isTesting=t;};
    void        setIsDebugging(bool b){isDebugging=b;};
    
    /// CLASS PARAMS
    /////////////////
    bool                isDebugging;
    bool                isTesting;
    
    vector<pmWarpPiRendererDrawable*> screenRenderers;
};



#endif /* pmWarpPiScreen_h */
