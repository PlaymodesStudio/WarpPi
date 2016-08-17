#include "pmWarpPiRendererVideoPlayer.h"


//-------------------------------------------------------------------------
pmWarpPiRendererVideoPlayer::pmWarpPiRendererVideoPlayer()
{
    videoFileName = "";
    elementDebugInfoPos = ofVec2f(520,20);

    
    type = renderVideo;
}
//-------------------------------------------------------------------------
void pmWarpPiRendererVideoPlayer::setupVideoPlayer(string _name, bool active)
{
    pmWarpPiRendererDrawable::setupScreen();
    
    createPlayer();
    /// VIDEO PLAYER

    videoFileName = _name;
    isTesting=false;
    activePlayer = active;
    
    loadMovie();
    setPlayerLoop(false);

    /// GUI
    if(!guiIsSetup)
    {
        gui->setup(); // most of the time you don't need a name but don't forget to call setup
        gui->add(screenOpacity.set( "opacity", 1.0, 0.0, 1.0));
        gui->add(maxScreenOpacity.set( "max opacity", 1.0, 0.0, 1.0));
        gui->setPosition(520, 20 + 75);
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
        this->setPlayerVolume(audioVolume);
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
    
    if(true)// It will be for me it they send it (address=="/all")||(id==addressWithoutSlash))
    {
        /// THIS MESSAGE IF FOR YOU !!
        
        /// COMMAND
        string command = m->getArgAsString(0);
        
        
        /// Change Volume
        if(command == "volume")
            this->setPlayerVolume(m->getArgAsFloat(1));
        
        else if(command == "loop")
            this->setPlayerLoop(m->getArgAsBool(1));
            
        /// PLAY
        else if(command == "playVideo")
        {
            if (isPlayerPaused() && !isFading){
                float fade = m->getArgAsFloat(1);
                //videoPlayer.play();
                if(!activePlayer){
                    activePlayer = true;
                    ofNotifyEvent(swapEvent, fade, this);
                }
                
                setPlayerPaused(false);
                Tweenzor::add((float *)&screenOpacity.get(), 0.0, maxScreenOpacity, 0.0, fade, EASE_IN_OUT_EXPO);
                Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererVideoPlayer::onComplete);
                isFading = true;
            }
        }
        /// STOP
        else if(command == "stopVideo")
        {
            if(!isPlayerPaused() && !isFading){
                Tweenzor::add((float *)&screenOpacity.get(), screenOpacity, 0.0, 0.0, m->getArgAsFloat(1),EASE_IN_OUT_EXPO);
                Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererVideoPlayer::onComplete);
                isFading = true;
            }
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
            if(!isFading){
                restartMovie();
                Tweenzor::add((float *)&screenOpacity.get(), 0.0, maxScreenOpacity, 0.0, m->getArgAsFloat(1),EASE_IN_OUT_EXPO);
                Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererVideoPlayer::onComplete);
                isFading = true;
            }
        }
        /// LOAD MOVIE
        else if(command == "loadVideo")
        {
            if (!isFading){
                auto new_videoFileName = "./videos/" +  m->getArgAsString(1);
                ofFile videoCheck(new_videoFileName);
                if(videoFileName != new_videoFileName && videoCheck.exists()){
                    fadeTime = m->getArgAsFloat(2);
                    
                    if(!activePlayer){
                        activePlayer = true;
                        ofNotifyEvent(swapEvent, fadeTime, this);
                    }
                    //            videoFileName = ofToDataPath("./videos", true);
                    videoFileName = new_videoFileName;
                    
                    ofLog(OF_LOG_NOTICE) << "pmOmxPlayer :: OSC :: load : " << videoFileName << " : fadeTime : " << fadeTime;
                    
                    Tweenzor::add((float *)&screenOpacity.get(), maxScreenOpacity, 0.0, 0.0, fadeTime, EASE_IN_OUT_EXPO);
                    Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererVideoPlayer::onFadeOutComplete);
                    isFading = true;
                }
            }
        }
    }
    
    pmWarpPiRendererDrawable::updateOSC(m);
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
    
    isFading = false;
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
    int whichHeight = elementDebugInfoPos.y;
    ofSetColor(255,128,0);
    ofDrawBitmapString("VIDEO PLAYER",elementDebugInfoPos.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofSetColor(255);
    ofDrawBitmapString(videoFileName,elementDebugInfoPos.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofDrawBitmapString(ofToString(getPlayerWidth()) + " x " +ofToString(getPlayerHeight()),elementDebugInfoPos.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofDrawBitmapString(ofToString(getPlayerCurrentFrame()) + " / " +ofToString(getPlayerTotalNumFrames()),elementDebugInfoPos.x,whichHeight);
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
    
    ofDrawBitmapString(loopType,elementDebugInfoPos.x,whichHeight);
}


//-------------------------------------------------------------------------
void pmWarpPiRendererVideoPlayer::stopVideoPlayer(float _fadeTime)
{
    activePlayer = false;
    Tweenzor::add((float *)&screenOpacity.get(), screenOpacity, 0.0, 0.0, _fadeTime ,EASE_IN_OUT_EXPO);
    Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererVideoPlayer::onComplete);
}

//-------------------------------------------------------------------------
void pmWarpPiRendererVideoPlayer::startVideoPlayer(float _fadeTime)
{
    
}