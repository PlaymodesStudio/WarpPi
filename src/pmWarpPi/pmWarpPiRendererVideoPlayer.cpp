#include "pmWarpPiRendererVideoPlayer.h"


//-------------------------------------------------------------------------
pmWarpPiRendererVideoPlayer::pmWarpPiRendererVideoPlayer()
{
    videoFileName = "";
    elementDebugInfoPos = ofVec2f(420,20);

    
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
//    ofLog(OF_LOG_NOTICE) << "RendVideoPlayer::update";
    
    /// TWEENZOR
    Tweenzor::update( ofGetElapsedTimeMillis() );
    screenOpacity = screenOpacity; //Why?
    
    
    if(isTesting)
    {
        this->setPlayerVolume(0);
    }
    else
    {
        if(isFading)
            this->setPlayerVolume(screenOpacity * audioVolume);
        else
            this->setPlayerVolume(audioVolume);
    }
    this->updatePlayer();
}

//-------------------------------------------------------------------------
void pmWarpPiRendererVideoPlayer::drawElement(ofRectangle container)
{
//    ofLog(OF_LOG_NOTICE) << "RendVideoPlayer::draw";
    
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
    ofLog(OF_LOG_NOTICE) << "VideoPlayer-OSC" << endl;
    
    
    string address = m->getAddress();
    
    // get the id
    string addressWithoutSlash = address.substr(1,address.size()-1);
    
    if(true)// It will be for me it they send it (address=="/all")||(id==addressWithoutSlash))
    {
        /// THIS MESSAGE IF FOR YOU !!
        
        /// COMMAND
        string command = m->getArgAsString(0);
        
        
        /// Change Volume
        if(command == "volume"){
            audioVolume = m->getArgAsFloat(1);
        }
        else if(command == "loop"){
            this->setPlayerLoop(m->getArgAsBool(1));
            cout<< "Set loop State: " << m->getArgAsBool(1) << endl;
            ofLog(OF_LOG_NOTICE) << "Set Loop State: " << loop;
        }
        
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
                Tweenzor::addCompleteListener(Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererVideoPlayer::onComplete);
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
                if(videoCheck.exists()){
                    cout<<"Loading Video: " << new_videoFileName << endl;
                    fadeTime = m->getArgAsFloat(2);
                    videoFileName = new_videoFileName;
                    
                    if(!activePlayer){
                        activePlayer = true;
                        ofNotifyEvent(swapEvent, fadeTime, this);
                        loadMovie();
                        Tweenzor::add((float *)&screenOpacity.get(), 0.0, maxScreenOpacity, 0.0, fadeTime, EASE_IN_OUT_EXPO);
                        Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererVideoPlayer::onComplete);
                    }
                    else{
                        Tweenzor::add((float *)&screenOpacity.get(), maxScreenOpacity, 0.0, 0.0, fadeTime, EASE_IN_OUT_EXPO);
                        Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererVideoPlayer::onFadeOutComplete);
                        isFading = true;
                    }
                    
                    ofLog(OF_LOG_NOTICE) << "VideoPlayer-LoadVideo: " << videoFileName << " - fadeTime: " << fadeTime;
                }
                else{
                    ofLog(OF_LOG_ERROR) << "VideoPlayer - No VideoFile";
                    cout<< "No Videofile: " << new_videoFileName << endl;
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
    //cout << "screen :: onComplete : arg = " << *arg << endl;
    
    if(arg == &screenOpacity.get())
    {
        //cout << "this is a stop?" << endl;
        if(screenOpacity==0.0) stopPlayer();
    }
    
    isFading = false;
}

//--------------------------------------------------------------
void pmWarpPiRendererVideoPlayer::onFadeOutComplete(float* arg)
{
    // this function is called on when the tween is complete //
    //cout << "screen :: FadeOut Completed : arg = " << *arg << endl;
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
    ofDrawBitmapString("PATH: " + videoFileName,elementDebugInfoPos.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofDrawBitmapString("SIZE: " + ofToString(getPlayerWidth()) + " x " +ofToString(getPlayerHeight()),elementDebugInfoPos.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofDrawBitmapString("FRAME: " + ofToString(getPlayerCurrentFrame()) + " / " +ofToString(getPlayerTotalNumFrames()),elementDebugInfoPos.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    string loopType;
    
    if(getPlayerLoopState()==OF_LOOP_NONE)
    {
        loopType = "no";
    }
    else if (getPlayerLoopState()== OF_LOOP_NORMAL);
    {
        loopType = "yes";
    }
    
    ofDrawBitmapString("LOOP: " + loopType,elementDebugInfoPos.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofDrawBitmapString("OPACITY: " + ofToString(screenOpacity.get()), elementDebugInfoPos.x, whichHeight);
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
