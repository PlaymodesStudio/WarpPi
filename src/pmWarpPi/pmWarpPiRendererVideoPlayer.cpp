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
    if(useFbo)
        screenFbo->begin();
    //----
 
    
        // SCREEN BACKGROUND
        ofSetColor(0,0,0,255);
        ofFill();
        ofDrawRectangle(0,0,screenSize.x,screenSize.y);
        
        if(isTesting)
        {
            ofSetColor(255);
            testingImage->draw(videoPosition.x,videoPosition.y,videoSize.x,videoSize.y);
        }
        else
        {
            // DRAW VIDEO
            ofSetColor(255 * screenOpacity * maxScreenOpacity);
            drawPlayer();
        }
    //----
    if(useFbo)
        screenFbo->end();
    //----
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
        if(command == "play")
        {
            //videoPlayer.play();
            setPlayerPaused(false);
            Tweenzor::add((float *)&screenOpacity.get(), 0.0, maxScreenOpacity, 0.0, m->getArgAsFloat(1),EASE_IN_OUT_EXPO);
            Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererVideoPlayer::onComplete);
        }
        /// STOP
        else if(command == "stop")
        {
            Tweenzor::add((float *)&screenOpacity.get(), maxScreenOpacity, 0.0, 0.0, m->getArgAsFloat(1),EASE_IN_OUT_EXPO);
            Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererVideoPlayer::onComplete);
        }
        /// PAUSE
        else if(command == "pause")
        {
            if(isPlayerPaused()) setPlayerPaused(false);
            else setPlayerPaused(true);
        }
        /// RESTART
        else if(command == "restart")
        {
            restartMovie();
            Tweenzor::add((float *)&screenOpacity.get(), 0.0, maxScreenOpacity, 0.0, m->getArgAsFloat(1),EASE_IN_OUT_EXPO);
            Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererVideoPlayer::onComplete);
        }
        /// LOAD MOVIE
        else if(command == "load")
        {
            auto new_videoFileName = m->getArgAsString(1);
            fadeTime = m->getArgAsFloat(2);
            
//            videoFileName = ofToDataPath("./videos", true);
            new_videoFileName = "./videos/"+new_videoFileName;
            if(videoFileName == new_videoFileName) return;
            videoFileName = new_videoFileName;
            
            ofLog(OF_LOG_NOTICE) << "pmOmxPlayer :: OSC :: load : " << videoFileName << " : fadeTime : " << fadeTime;
            
//            loadMovie();
            
            Tweenzor::add((float *)&screenOpacity.get(), maxScreenOpacity, 0.0, 0.0, fadeTime, EASE_IN_OUT_EXPO);
            Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererVideoPlayer::onFadeOutComplete);
            
//            Tweenzor::add((float *)&screenOpacity.get(), 0.0, maxScreenOpacity, 0.0, fadeTime,EASE_IN_OUT_EXPO);
//            Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererVideoPlayer::onComplete);
            
        }
        
        /// EDIT QUAD
        else if(command == "editQuad")
        {
            cout << "OMXPLAYER received a doEditQuad !! " << endl;
            
            int val = m->getArgAsInt32(1);
            if(val==0)
            {
                doEditQuadPoints = false;
            }
            else
            {
                doEditQuadPoints = true;
            }
        }
        /// NEXT QUAD POINT
        else if(command == "nextQuadPoint")
        {
            nextQuadPoint();
        }
        /// PREVIOUS QUAD POINT
        else if(command == "preQuadPoint")
        {
            previousQuadPoint();
        }
        /// MOVE QUAD POINT UP
        else if(command == "movePointUp")
        {
            distortedCorners[currentQuadPoint].y = distortedCorners[currentQuadPoint].y - m->getArgAsInt32(1);
        }
        /// MOVE QUAD POINT DOWN
        else if(command == "movePointDown")
        {
            distortedCorners[currentQuadPoint].y = distortedCorners[currentQuadPoint].y + m->getArgAsInt32(1);
        }
        /// MOVE QUAD POINT LEFT
        else if(command == "movePointLeft")
        {
            distortedCorners[currentQuadPoint].x = distortedCorners[currentQuadPoint].x - m->getArgAsInt32(1);
        }
        /// MOVE QUAD POINT RIGHT
        else if(command == "movePointRight")
        {
            distortedCorners[currentQuadPoint].x = distortedCorners[currentQuadPoint].x + m->getArgAsInt32(1);
        }
        /// SAVE QUAD
        else if(command == "saveQuad")
        {
            saveConfigToXML();
        }
        /// RESET QUAD
        else if(command == "resetQuad")
        {
            cout << "Resetting Quad !!" << endl;
            resetQuad();
        }
        
        
        
    }
    
    pmWarpPiRendererScreen::updateOSC(m);
    
//    if(address.find("play")!=-1)
//    {
//        playPlayer();
//        Tweenzor::add((float *)&screenOpacity.get(), 0.0, 1.0, 0.0, m->getArgAsFloat(0),EASE_IN_OUT_EXPO);
//        Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererVideoPlayer::onComplete);
//    }
//    if(address.find("stop")!=-1)
//    {
//        Tweenzor::add((float *)&screenOpacity.get(), 1.0, 0.0, 0.0, m->getArgAsFloat(0),EASE_IN_OUT_EXPO);
//        Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererVideoPlayer::onComplete);
//    }
//    if(address.find("pause")!=-1)
//    {
//        if(isPlayerPaused()) setPlayerPaused(false);
//        else setPlayerPaused(true);
//        
//    }
//    if(address.find("restart")!=-1)
//    {
//        setPlayerPaused(true);
//        setPlayerPosition(0.0);
//        playPlayer();
//        Tweenzor::add((float *)&screenOpacity.get(), 0.0, 1.0, 0.0, m->getArgAsFloat(0),EASE_IN_OUT_EXPO);
//        Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererVideoPlayer::onComplete);
//    }
    
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

//--------------------------------------------------------------
void pmWarpPiRendererVideoPlayer::onFadeOutComplete(float* arg)
{
    // this function is called on when the tween is complete //
    cout << "screen :: FadeOut Completed : arg = " << *arg << endl;
    loadMovie();
    Tweenzor::add((float *)&screenOpacity.get(), 0.0, maxScreenOpacity, 0.0, fadeTime, EASE_IN_OUT_EXPO);
    Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererVideoPlayer::onComplete);
    
//    if(arg == &screenOpacity.get())
//    {
//        cout << "this is a stop?" << endl;
//        if(screenOpacity==0.0) stopPlayer();
//    }
//    //    if(arg == &_x2.get())
//    //    {
//    //        Tweenzor::add((float *)&_x1.get(), 0, ofGetWidth(), 0, 2,EASE_IN_OUT_EXPO);
//    //        Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&_x1.get()), this, &testApp::onComplete);
//    //    }
    
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