#include "pmWarpPiRenderer.h"

/*
ofAddListener(ofEvents.mousePressed, this, &MyClass::_mousePressed);
ofAddListener(ofEvents.mouseMoved, this, &MyClass::_mouseMoved);
ofAddListener(ofEvents.mouseDragged, this, &MyClass::_mouseDragged);
ofAddListener(ofEvents.mouseReleased, this, &MyClass::_mouseReleased);
ofAddListener(ofEvents.keyPressed, this, &MyClass::_keyPressed);
ofAddListener(ofEvents.keyReleased, this, &MyClass::_keyReleased);
ofAddListener(ofEvents.setup, this, &MyClass::_setup);
ofAddListener(ofEvents.update, this, &MyClass::_update);
ofAddListener(ofEvents.draw, this, &MyClass::_draw);
ofAddListener(ofEvents.exit, this, &MyClass::_exit);
*/

//-------------------------------------------------------------------------
pmWarpPiRenderer::pmWarpPiRenderer()
{
    isDebugging = false;
    oscSender = new ofxOscSender();
    gui = new ofxPanel();
    guiIsSetup = false;
    
    cout << "pmRenderer :: new renderer ";

}

//-------------------------------------------------------------------------
void pmWarpPiRenderer::setup(string _id)
{
    ofAddListener(ofEvents().update, this, &pmWarpPiRenderer::update);
    id = _id;
    /// DEBUGGING
    isDebugging = false;
//    ofLog(OF_LOG_NOTICE) << "Rend :: setup :: osc sender " << oscSenderAddress << " :: Port " << oscSenderPort;
}


//-------------------------------------------------------------------------
void pmWarpPiRenderer::update(ofEventArgs & a)
{
//    ofLog(OF_LOG_NOTICE) << "Rend::update";

}

//-------------------------------------------------------------------------
void pmWarpPiRenderer::updateOSC(ofxOscMessage* m)
{
//    ofLog(OF_LOG_NOTICE) << "Rend::update OSC";
    
}

//-------------------------------------------------------------------------
void pmWarpPiRenderer::deleteRenderer()
{
//    ofLog(OF_LOG_NOTICE) << "Rend::deleteRenderer";

}


