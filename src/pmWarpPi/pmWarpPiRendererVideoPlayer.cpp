#include "pmWarpPiRendererVideoPlayer.h"


//-------------------------------------------------------------------------
pmWarpPiRendererVideoPlayer::pmWarpPiRendererVideoPlayer()
{
    videoFileName = "";
    videoPosition = ofVec2f(0,0);
    videoSize = ofVec2f(0,0);
    videoPlayerDebugPosition = ofVec2f(520,20);
    
    type = renderVideo;
}
//-------------------------------------------------------------------------
void pmWarpPiRendererVideoPlayer::setupVideoPlayer(string _name,ofVec2f _pos, ofVec2f _size, bool active)
{
    createPlayer();
    /// VIDEO PLAYER

    elementRect.set(_pos, _size.x, _size.y);
    videoFileName = _name;
    videoPosition = _pos;
    videoSize = _size;
    isTesting=false;
    activePlayer = active;
    
    loadMovie();

    /// GUI
    if(!guiIsSetup)
    {
        gui->setup(); // most of the time you don't need a name but don't forget to call setup
        gui->add(screenOpacity.set( "opacity", 1.0, 0.0, 1.0));
        gui->add(maxScreenOpacity.set( "max opacity", 1.0, 0.0, 1.0));
        gui->setPosition(videoPlayerDebugPosition.x,videoPlayerDebugPosition.y + 75);
        guiIsSetup = true;
    }
}

//-------------------------------------------------------------------------
void pmWarpPiRendererVideoPlayer::updateForScreen()
{
    
    ofLog(OF_LOG_NOTICE) << "RendVideoPlayer::update";
    
    /// TWEENZOR
    Tweenzor::update( ofGetElapsedTimeMillis() );
    screenOpacity = screenOpacity; //Why?
    
    
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
void pmWarpPiRendererVideoPlayer::drawElement(ofRectangle container)
{
    ofLog(OF_LOG_NOTICE) << "RendVideoPlayer::draw";
    
    // SCREEN BACKGROUND
//    ofSetColor(0,0,0,255*screenOpacity);
//    ofFill();
    //ofDrawRectangle(container);
    
    // DRAW VIDEO
    ofSetColor(255,255,255,255 * screenOpacity * maxScreenOpacity);
    drawPlayer(container);
}


//-------------------------------------------------------------------------
void pmWarpPiRendererVideoPlayer::updateOSC(ofxOscMessage* m)
{
    ofLog(OF_LOG_NOTICE) << "VideoPlayer :: updateOSC :: " << m->getAddress();
    
    
    string address = m->getAddress();
    
    // get the id
    string addressWithoutSlash = address.substr(1,address.size()-1);
    
    if((address=="/all")||(id==addressWithoutSlash))
    {
        /// THIS MESSAGE IF FOR YOU !!
        
        /// COMMAND
        string command = m->getArgAsString(0);
        
        /// PLAY
        if(command == "playVideo")
        {
            float fade = m->getArgAsFloat(1);
            //videoPlayer.play();
            if(!activePlayer){
                activePlayer = true;
                ofNotifyEvent(swapEvent, fade, this);
            }
            
            setPlayerPaused(false);
            Tweenzor::add((float *)&screenOpacity.get(), 0.0, maxScreenOpacity, 0.0, fade, EASE_IN_OUT_EXPO);
            Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererVideoPlayer::onComplete);
        }
        /// STOP
        else if(command == "stopVideo")
        {
            Tweenzor::add((float *)&screenOpacity.get(), maxScreenOpacity, 0.0, 0.0, m->getArgAsFloat(1),EASE_IN_OUT_EXPO);
            Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererVideoPlayer::onComplete);
        }
        /// PAUSE
        else if(command == "pauseVideo")
        {
            if(isPlayerPaused()) setPlayerPaused(false);
            else setPlayerPaused(true);
        }
        /// RESTART
        else if(command == "restartVideo")
        {
            restartMovie();
            Tweenzor::add((float *)&screenOpacity.get(), 0.0, maxScreenOpacity, 0.0, m->getArgAsFloat(1),EASE_IN_OUT_EXPO);
            Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererVideoPlayer::onComplete);
        }
        /// LOAD MOVIE
        else if(command == "loadVideo")
        {
            
           
            auto new_videoFileName = m->getArgAsString(1);
            fadeTime = m->getArgAsFloat(2);
            
            if(!activePlayer){
                activePlayer = true;
                ofNotifyEvent(swapEvent, fadeTime, this);
            }
            
//            videoFileName = ofToDataPath("./videos", true);
            new_videoFileName = "./videos/"+new_videoFileName;
            if(videoFileName == new_videoFileName) return;
            videoFileName = new_videoFileName;
            
            ofLog(OF_LOG_NOTICE) << "pmOmxPlayer :: OSC :: load : " << videoFileName << " : fadeTime : " << fadeTime;
            
            Tweenzor::add((float *)&screenOpacity.get(), maxScreenOpacity, 0.0, 0.0, fadeTime, EASE_IN_OUT_EXPO);
            Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererVideoPlayer::onFadeOutComplete);
        }
    }
    
    pmWarpPiRendererScreen::updateOSC(m);
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
}

//--------------------------------------------------------------
void pmWarpPiRendererVideoPlayer::onFadeOutComplete(float* arg)
{
    // this function is called on when the tween is complete //
    cout << "screen :: FadeOut Completed : arg = " << *arg << endl;
    loadMovie();
    Tweenzor::add((float *)&screenOpacity.get(), 0.0, maxScreenOpacity, 0.0, fadeTime, EASE_IN_OUT_EXPO);
    Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererVideoPlayer::onComplete);
    
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
void pmWarpPiRendererVideoPlayer::stopVideoPlayer(float _fadeTime)
{
    activePlayer = false;
    Tweenzor::add((float *)&screenOpacity.get(), maxScreenOpacity, 0.0, 0.0, _fadeTime ,EASE_IN_OUT_EXPO);
    Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererVideoPlayer::onComplete);
}

//-------------------------------------------------------------------------
void pmWarpPiRendererVideoPlayer::startVideoPlayer(float _fadeTime)
{
    
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