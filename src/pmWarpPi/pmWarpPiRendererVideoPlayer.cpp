#include "pmWarpPiRendererVideoPlayer.h"


//-------------------------------------------------------------------------
pmWarpPiRendererVideoPlayer::pmWarpPiRendererVideoPlayer()
{
    videoFileName = "";
    videoPosition = ofVec2f(0,0);
    videoSize = ofVec2f(0,0);
    videoPlayerDebugPosition = ofVec2f(520,20);
}
//-------------------------------------------------------------------------
void pmWarpPiRendererVideoPlayer::setupVideoPlayer(string _name,ofVec2f _pos, ofVec2f _size)
{
    createPlayer();
    /// VIDEO PLAYER

    videoFileName = _name;
    videoPosition = _pos;
    videoSize = _size;
    isTesting=false;
    
    loadAndPlay();

    /// GUI
    gui->setup(); // most of the time you don't need a name but don't forget to call setup
	gui->add(screenOpacity.set( "opacity", 1.0, 0.0, 1.0));
    gui->setPosition(videoPlayerDebugPosition.x,videoPlayerDebugPosition.y + 75);
    
}

//-------------------------------------------------------------------------
void pmWarpPiRendererVideoPlayer::updateForScreen()
{
    
    ofLog(OF_LOG_NOTICE) << "RendVideoPlayer::update";
    
    /// TWEENZOR
    Tweenzor::update( ofGetElapsedTimeMillis() );
    screenOpacity = screenOpacity;
    
    
    if(isTesting)
    {
        this->setPlayerVolume(0);
    }
    else
    {
        this->setPlayerVolume(screenOpacity);
        this->updatePlayer();
    }
    
}

//-------------------------------------------------------------------------
void pmWarpPiRendererVideoPlayer::drawIntoFbo()
{
    ofLog(OF_LOG_NOTICE) << "RendVideoPlayer::draw";
    if(isDebugging)
    {
        showScreenDebug();
        showDebug();
    }
    else
    {
    }
    //----
    screenFbo->begin();
    //----
 
    
        // SCREEN BACKGROUND
        ofSetColor(0,0,0,255);
        ofFill();
        ofRect(0,0,screenSize.x,screenSize.y);
        
        if(isTesting)
        {
            ofSetColor(255);
            testingImage->draw(videoPosition.x,videoPosition.y,videoSize.x,videoSize.y);
        }
        else
        {
            // DRAW VIDEO
            ofSetColor(255 * screenOpacity);
            drawPlayer();
        }
    //----
    screenFbo->end();
    //----
}


//-------------------------------------------------------------------------
void pmWarpPiRendererVideoPlayer::updateOSC(ofxOscMessage* m)
{
    ofLog(OF_LOG_NOTICE) << "RendVideoPlayer::updateOSC";
    
    
    string address = m->getAddress();
    
    if(address.find("play")!=-1)
    {
        playPlayer();
        Tweenzor::add((float *)&screenOpacity.get(), 0.0, 1.0, 0.0, m->getArgAsFloat(0),EASE_IN_OUT_EXPO);
        Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererVideoPlayer::onComplete);
    }
    if(address.find("stop")!=-1)
    {
        Tweenzor::add((float *)&screenOpacity.get(), 1.0, 0.0, 0.0, m->getArgAsFloat(0),EASE_IN_OUT_EXPO);
        Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererVideoPlayer::onComplete);
    }
    if(address.find("pause")!=-1)
    {
        if(isPlayerPaused()) setPlayerPaused(false);
        else setPlayerPaused(true);
        
    }
    if(address.find("restart")!=-1)
    {
        setPlayerPaused(true);
        setPlayerPosition(0.0);
        playPlayer();
        Tweenzor::add((float *)&screenOpacity.get(), 0.0, 1.0, 0.0, m->getArgAsFloat(0),EASE_IN_OUT_EXPO);
        Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererVideoPlayer::onComplete);
    }
    
}

//--------------------------------------------------------------
void pmWarpPiRendererVideoPlayer::onComplete(float* arg)
{
    // this function is called on when the tween is complete //
    cout << "screen :: onComplete : arg = " << *arg << endl;
    
    if(arg == &screenOpacity.get())
    {
        cout << "this is a stop?" << endl;
        if(screenOpacity==0.0) stopPlayer();
    }
    //    if(arg == &_x2.get())
    //    {
    //        Tweenzor::add((float *)&_x1.get(), 0, ofGetWidth(), 0, 2,EASE_IN_OUT_EXPO);
    //        Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&_x1.get()), this, &testApp::onComplete);
    //    }
    
}

//-------------------------------------------------------------------------
void pmWarpPiRendererVideoPlayer::deleteRenderer()
{
    ofLog(OF_LOG_NOTICE) << "RendVideoPlayer::delete";
    
    closePlayer();
    
}

//-------------------------------------------------------------------------
void pmWarpPiRendererVideoPlayer::showDebug()
{
        
    int lineHeight = 15;
    int whichHeight = videoPlayerDebugPosition.y;
    ofSetColor(255,128,0);
    ofDrawBitmapString("VIDEO PLAYER",videoPlayerDebugPosition.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofSetColor(255);
    ofDrawBitmapString(videoFileName,videoPlayerDebugPosition.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofDrawBitmapString(ofToString(getPlayerWidth()) + " x " +ofToString(getPlayerHeight()),videoPlayerDebugPosition.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofDrawBitmapString(ofToString(getPlayerCurrentFrame()) + " / " +ofToString(getPlayerTotalNumFrames()),videoPlayerDebugPosition.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    string loopType;
    
    if(getPlayerLoopState()==OF_LOOP_NONE)
    {
        loopType = "no loop";
    }
    else if (getPlayerLoopState()== OF_LOOP_NORMAL);
    {
        loopType = "loop";
    }
    
    ofDrawBitmapString(loopType,videoPlayerDebugPosition.x,whichHeight);
    
    
}

//-------------------------------------------------------------------------
void pmWarpPiRendererVideoPlayer::keyPressed(ofKeyEventArgs &a)
{
    int key = a.key;
    
    if(key=='p')
    {
        playPlayer();
    }
    
    cout << "videoplayer key pressed " << key << endl;
    
}
//-------------------------------------------------------------------------
void pmWarpPiRendererVideoPlayer::keyReleased(ofKeyEventArgs &a)
{
    
}
//-------------------------------------------------------------------------
void pmWarpPiRendererVideoPlayer::mouseMoved(ofMouseEventArgs &a)
{
}
//-------------------------------------------------------------------------
void pmWarpPiRendererVideoPlayer::mouseDragged(ofMouseEventArgs &a)
{
    
}
//-------------------------------------------------------------------------
void pmWarpPiRendererVideoPlayer::mousePressed(ofMouseEventArgs &a)
{
    
}
//-------------------------------------------------------------------------
void pmWarpPiRendererVideoPlayer::mouseReleased(ofMouseEventArgs &a)
{
    
}