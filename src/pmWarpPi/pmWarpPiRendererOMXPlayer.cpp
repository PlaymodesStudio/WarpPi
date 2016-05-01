//#include "pmWarpPiRendererOMXPlayer.h"
//
//#ifdef TARGET_RASPBERRY_PI
//
//
//int quadStep = 1;
//
////-------------------------------------------------------------------------
//pmWarpPiRendererOMXPlayer::pmWarpPiRendererOMXPlayer()
//{
//    videoFileName = "";
//    videoPosition = ofVec2f(0,0);
//    videoSize = ofVec2f(0,0);
//    videoPlayerDebugPosition = ofVec2f(520,20);
//
//    omxPlayer = new ofxOMXPlayer();
//    
//    ofLog(OF_LOG_NOTICE) << "pmOmxPlayer :: New OmxPlayer";
//}
//
////-------------------------------------------------------------------------
//void pmWarpPiRendererOMXPlayer::setupOMXPlayer(string _name,ofVec2f _pos, ofVec2f _size)
//{
//    /// VIDEO PLAYER
//    videoFileName = _name;
//    videoPosition = _pos;
//    videoSize = _size;
//    isTesting=false;
//
//	string videoPath = ofToDataPath(videoFileName, true);
//
//	//Somewhat like ofFboSettings we may have a lot of options so this is the current model
//	ofxOMXPlayerSettings settings;
//	settings.videoPath = videoPath;
//	settings.useHDMIForAudio	= false;		//default true
//	settings.enableTexture		= true;		//default true
//	settings.enableLooping		= true;		//default true
//	settings.enableAudio		= true;		//default true, save resources by disabling
//	//settings.doFlip     Texture = true;		//default false
//
//
//	if (!settings.enableTexture)
//	{
//		/*
//		 We have the option to pass in a rectangle
//		 to be used for a non-textured player to use (as opposed to the default full screen)
//		 */
//		settings.displayRect.width = videoSize.x;
//		settings.displayRect.height = videoSize.y;
//		settings.displayRect.x = 0;
//		settings.displayRect.y = 0;
//	}
//
//	//so either pass in the settings
//	omxPlayer->setup(settings);
//
//    //??videoSize = ofVec2f(omxPlayer->getWidth(),omxPlayer->getHeight());
//	
//    /// GUI
//    if(!guiIsSetup)
//    {
//        gui->setup(); // most of the time you don't need a name but don't forget to call setup
//        gui->add(screenOpacity.set( "opacity", 1.0, 0.0, 1.0));
//        gui->add(maxScreenOpacity.set( "max opacity", 1.0, 0.0, 1.0));
//        gui->setPosition(videoPlayerDebugPosition.x,videoPlayerDebugPosition.y + 75);
//        guiIsSetup = true;
//    }
//}
//
////-------------------------------------------------------------------------
//void pmWarpPiRendererOMXPlayer::updateForScreen()
//{
//    
//    //ofLog(OF_LOG_NOTICE) << "RendVideoPlayer::update";
//
//    /// TWEENZOR
//    Tweenzor::update( ofGetElapsedTimeMillis() );
//    screenOpacity = screenOpacity;
//
//}
//
////-------------------------------------------------------------------------
//void pmWarpPiRendererOMXPlayer::drawElement()
//{
//    if(isDebugging)
//    {
//        showScreenDebug();
//        showDebug();
//    }
//    else
//    {
//    }
//
//    //----
//    screenFbo->begin();
//    //----
//
//    ofSetColor(0,0,0,255);
//    ofFill();
//    ofRect(0,0,screenSize.x,screenSize.y);
//
//    if(isTesting)
//    {
//        ofSetColor(255);
//        testingImage->draw(videoPosition.x,videoPosition.y,videoSize.x,videoSize.y);
//    }
//    else
//    {
//		if(!omxPlayer->isTextureEnabled())
//        {
//            return;
//        }
//		ofSetColor(255 * screenOpacity * maxScreenOpacity);
//		//ofHideCursor();
//		omxPlayer->draw(0, 0, ofGetWidth(), ofGetHeight());
//	}
//    
//    //----
//    screenFbo->end();
//    //----
//
//}
//
//
////-------------------------------------------------------------------------
//void pmWarpPiRendererOMXPlayer::updateOSC(ofxOscMessage* m)
//{
//    ofLog(OF_LOG_NOTICE) << "omxPlayer :: processOSC :: " << m->getAddress();
//
//    string address = m->getAddress();
//    
//    // get the id
//    string addressWithoutSlash = address.substr(1,address.size()-1);
//    
//    if((address=="/all")||(id==addressWithoutSlash))
//    {
//        /// THIS MESSAGE IF FOR YOU !!
//        
//        /// COMMAND
//        string command = m->getArgAsString(0);
//        
//        /// PLAY
//        if(command == "play")
//        {
//            //videoPlayer.play();
//            omxPlayer->setPaused(false);
//            Tweenzor::add((float *)&screenOpacity.get(), 0.0, maxScreenOpacity, 0.0, m->getArgAsFloat(1),EASE_IN_OUT_EXPO);
//            Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererOMXPlayer::onComplete);
//        }
//        /// STOP
//        else if(command == "stop")
//        {
//            Tweenzor::add((float *)&screenOpacity.get(), maxScreenOpacity, 0.0, 0.0, m->getArgAsFloat(1),EASE_IN_OUT_EXPO);
//            Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererOMXPlayer::onComplete);
//        }
//        /// PAUSE
//        else if(command == "pause")
//        {
//            if(omxPlayer->isPaused()) omxPlayer->setPaused(false);
//            else omxPlayer->setPaused(true);
//        }
//        /// RESTART
//        else if(command == "restart")
//        {
//            omxPlayer->restartMovie();
//            Tweenzor::add((float *)&screenOpacity.get(), 0.0, maxScreenOpacity, 0.0, m->getArgAsFloat(1),EASE_IN_OUT_EXPO);
//            Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererOMXPlayer::onComplete);
//        }
//        /// LOAD MOVIE
//        else if(command == "load")
//        {
//            videoFileName = m->getArgAsString(1);
//            float fadeTime = m->getArgAsFloat(2);
//            
//            string pathToVideos = ofToDataPath("./videos", true);
//            string absoluteVideoFileName = ofToString(pathToVideos +"/" +videoFileName);
//            
//            ofLog(OF_LOG_NOTICE) << "pmOmxPlayer :: OSC :: load : " << absoluteVideoFileName << " : fadeTime : " << fadeTime;
//            
//            omxPlayer->loadMovie(absoluteVideoFileName);
//            
//            Tweenzor::add((float *)&screenOpacity.get(), 0.0, maxScreenOpacity, 0.0, fadeTime,EASE_IN_OUT_EXPO);
//            Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererOMXPlayer::onComplete);
//
//        }
//        /// EDIT QUAD
//        else if(command == "editQuad")
//        {
//            cout << "OMXPLAYER received a doEditQuad !! " << endl;
//            
//            int val = m->getArgAsInt32(1);
//            if(val==0)
//            {
//                doEditQuadPoints = false;
//            }
//            else
//            {
//                doEditQuadPoints = true;
//            }
//        }
//        /// NEXT QUAD POINT
//        else if(command == "nextQuadPoint")
//        {
//            nextQuadPoint();
//        }
//        /// PREVIOUS QUAD POINT
//        else if(command == "preQuadPoint")
//        {
//            previousQuadPoint();
//        }
//        /// MOVE QUAD POINT UP
//        else if(command == "movePointUp")
//        {
//            distortedCorners[currentQuadPoint].y = distortedCorners[currentQuadPoint].y - m->getArgAsInt32(1);
//        }
//        /// MOVE QUAD POINT DOWN
//        else if(command == "movePointDown")
//        {
//            distortedCorners[currentQuadPoint].y = distortedCorners[currentQuadPoint].y + m->getArgAsInt32(1);
//        }
//        /// MOVE QUAD POINT LEFT
//        else if(command == "movePointLeft")
//        {
//            distortedCorners[currentQuadPoint].x = distortedCorners[currentQuadPoint].x - m->getArgAsInt32(1);
//        }
//        /// MOVE QUAD POINT RIGHT
//        else if(command == "movePointRight")
//        {
//            distortedCorners[currentQuadPoint].x = distortedCorners[currentQuadPoint].x + m->getArgAsInt32(1);
//        }
//        /// SAVE QUAD
//        else if(command == "saveQuad")
//        {
//            saveConfigToXML();
//        }
//        /// RESET QUAD
//        else if(command == "resetQuad")
//        {
//            cout << "Resetting Quad !!" << endl;
//            resetQuad();
//        }
//        
//        
//        
//    }
//    
//    
//}
//
////--------------------------------------------------------------
//void pmWarpPiRendererOMXPlayer::onComplete(float* arg)
//{
//    // this function is called on when the tween is complete //
//    //cout << "screen :: onComplete : arg = " << *arg << endl;
//    ofLog(OF_LOG_VERBOSE) << "pmOmxPlayer :: on Complete!!";
//    
//        if(arg == &screenOpacity.get())
//        {
//            if(screenOpacity==0.0) omxPlayer->setPaused(true);
//        }
//    //    if(arg == &_x2.get())
//    //    {
//    //        Tweenzor::add((float *)&_x1.get(), 0, ofGetWidth(), 0, 2,EASE_IN_OUT_EXPO);
//    //        Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&_x1.get()), this, &testApp::onComplete);
//    //    }
//    
//}
//
////-------------------------------------------------------------------------
//void pmWarpPiRendererOMXPlayer::deleteRenderer()
//{
//    ofLog(OF_LOG_NOTICE) << "pmOmxPlayer :: on delete renderer!!";
//    
//    omxPlayer->close();
//
//}
//
////-------------------------------------------------------------------------
//void pmWarpPiRendererOMXPlayer::showDebug()
//{
//    int lineHeight = 15;
//    int whichHeight = videoPlayerDebugPosition.y;
//    ofSetColor(255,128,0);
//    ofDrawBitmapString("OMX PLAYER",videoPlayerDebugPosition.x,whichHeight);
//    whichHeight=whichHeight + lineHeight;
//    ofSetColor(255);
//    ofDrawBitmapString(videoFileName,videoPlayerDebugPosition.x,whichHeight);
//    whichHeight=whichHeight + lineHeight;
//    ofDrawBitmapString(ofToString(omxPlayer->getWidth()) + " x " +ofToString(omxPlayer->getHeight()),videoPlayerDebugPosition.x,whichHeight);
//    whichHeight=whichHeight + lineHeight;
//    ofDrawBitmapString(ofToString(omxPlayer->getCurrentFrame()) + " / " +ofToString(omxPlayer->getTotalNumFrames()),videoPlayerDebugPosition.x,whichHeight);
//    whichHeight=whichHeight + lineHeight;
//    
///*
//    string loopType;
//    
//    if(omxPlayer->getLoopState()==OF_LOOP_NONE)
//    {
//        loopType = "no loop";
//    }
//    else if (videoPlayer.getLoopState()== OF_LOOP_NORMAL);
//    {
//        loopType = "loop";
//    }
//    
//    ofDrawBitmapString(loopType,videoPlayerDebugPosition.x,whichHeight);
//    */
//    
//}
//
////-------------------------------------------------------------------------
//void pmWarpPiRendererOMXPlayer::testVideo(bool test)
//{
//	/*
//    if(test)
//    {
//        testomxPlayer->setPaused(false);
//        ofSetColor(255);
//        testomxPlayer->draw(screenPosition.x,screenPosition.y,screenSize.x,screenSize.y);
//    }
//    else testomxPlayer->setPaused(true);
//  */  
//    
//}
//
//#endif
