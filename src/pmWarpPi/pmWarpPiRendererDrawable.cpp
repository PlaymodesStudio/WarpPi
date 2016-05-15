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


//-------------------------------------------------------------------------
void pmWarpPiRendererDrawable::fadeIn(float fadeTime)
{
    Tweenzor::add((float*)&screenOpacity.get(), screenOpacity, maxScreenOpacity, 0.0, fadeTime, EASE_IN_OUT_EXPO);
    Tweenzor::addCompleteListener(Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererDrawable::onFadeComplete);
}

//-------------------------------------------------------------------------
void pmWarpPiRendererDrawable::fadeOut(float fadeTime)
{
    Tweenzor::add((float*)&screenOpacity.get(), screenOpacity, 0.0,  0.0, fadeTime, EASE_IN_OUT_EXPO);
    Tweenzor::addCompleteListener(Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererDrawable::onFadeComplete);
}

void pmWarpPiRendererDrawable::onFadeComplete(float *args)
{
    if(*args == 0.0){//screen has fade out
        cout<<"fadeOut done"<<endl;
    }
    else{
        cout<<"fadeIn complete"<<endl;
    }
}

//-------------------------------------------------------------------------
void pmWarpPiRendererDrawable::keyPressed(ofKeyEventArgs &a)
{
    
}
//-------------------------------------------------------------------------
void pmWarpPiRendererDrawable::keyReleased(ofKeyEventArgs &a)
{

}
//-------------------------------------------------------------------------
void pmWarpPiRendererDrawable::mouseMoved(ofMouseEventArgs &a)
{

}
//-------------------------------------------------------------------------
void pmWarpPiRendererDrawable::mouseDragged(ofMouseEventArgs &a)
{
    
}
//-------------------------------------------------------------------------
void pmWarpPiRendererDrawable::mousePressed(ofMouseEventArgs &a)
{
    
}
//-------------------------------------------------------------------------
void pmWarpPiRendererDrawable::mouseReleased(ofMouseEventArgs &a)
{
    
}

