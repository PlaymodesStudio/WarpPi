#include "pmWarpPiRendererDrawable.h"

//-------------------------------------------------------------------------
pmWarpPiRendererDrawable::pmWarpPiRendererDrawable()
{
    screenRect = ofRectangle(0,0, ofGetWidth(), ofGetHeight());
    elementRect = screenRect;
    elementDebugRect = ofRectangle(0,ofGetHeight()-ofGetHeight()/2,ofGetWidth()/2,ofGetHeight()/2);
    
    screenOpacity = 1.0;
    maxScreenOpacity = 1.0;
    
    cout << "pmRendererScreen :: new reder screen";
}

//-------------------------------------------------------------------------
void pmWarpPiRendererDrawable::setupScreen()
{
	Tweenzor::init();
}

//-------------------------------------------------------------------------
void pmWarpPiRendererDrawable::update(ofEventArgs & a)
{
    /// TWEENZOR
    Tweenzor::update( ofGetElapsedTimeMillis() );

    // all "tweened" values needs to be reassigned here !!
    screenOpacity = screenOpacity;

    //ofLog(OF_LOG_NOTICE) << "RendScreen::update";

    updateForScreen();
}

//-------------------------------------------------------------------------
void pmWarpPiRendererDrawable::draw()
{

}
//-------------------------------------------------------------------------
void pmWarpPiRendererDrawable::updateOSC(ofxOscMessage* m)
{
    
}

//-------------------------------------------------------------------------
void pmWarpPiRendererDrawable::deleteRenderer()
{
    ofLog(OF_LOG_NOTICE) << "RendScreen::delete";
    Tweenzor::destroy();
}

//-------------------------------------------------------------------------
void pmWarpPiRendererDrawable::showScreenDebug()
{
    int lineHeight = 15;
    int whichHeight = screenDebugPosition.y;
    
    ofSetColor(255,128,0);
    ofDrawBitmapString("SCREEN",screenDebugPosition.x,whichHeight);
    ofSetColor(255);
    whichHeight=whichHeight + lineHeight;
    ofDrawBitmapString("screen size : " +ofToString(screenSize),screenDebugPosition.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofDrawBitmapString("screen position : " +ofToString(screenPosition),screenDebugPosition.x,whichHeight);

}
