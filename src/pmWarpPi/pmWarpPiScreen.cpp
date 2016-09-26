//
//  pmWarpPiScreen.cpp
//  WarpPi
//
//  Created by Eduard Frigola on 12/05/16.
//
//

#include "pmWarpPiScreen.h"


//-------------------------------------------------------------------------
pmWarpPiScreen::pmWarpPiScreen()
{
    screenRect = ofRectangle(0,0, ofGetWidth(), ofGetHeight());
    elementRect = screenRect;
    elementDebugRect = ofRectangle(0,ofGetHeight()-ofGetHeight()/2,ofGetWidth()/2,ofGetHeight()/2);
    
    screenPosition = ofVec2f(0,0);
    screenSize = ofVec2f(0,0);
    doHomography = true;
    useFbo = false;
    doEditQuadPoints = false;
    isTesting = false;
    isDebugging= false;
    currentQuadPoint = 0;
    screenDebugPosition = ofVec2f(270,20);
    testingImage = new ofImage();
    homography = new ofMatrix4x4();
    screenFbo = new ofFbo();
    currentQuadPoint = 0;
}

//-------------------------------------------------------------------------
void pmWarpPiScreen::setup(bool hasVideo, bool hasImage, bool _useFbo, bool _useHomography, bool textureMode, bool hasAudio, bool loop, string initVideoFile, string initImageFile){
    
    useFbo = _useFbo;
    doHomography = _useHomography;
    
    /// RENDERERS
    ///////////////
    //    int resX = 1280;
    //    int resY = 720;
    int resX = ofGetWidth();
    int resY = ofGetHeight();
    
    // HAS VIDEO ?
    ///////////////
    if(hasVideo)
    {
#ifdef TARGET_RASPBERRY_PI
        pmWarpPiRendererOMXPlayer* _video = new pmWarpPiRendererOMXPlayer();
        _video->setTextured(textureMode);
        _video->setAudio(hasAudio);
        _video->setPlayerLoop(loop);
#else
        pmWarpPiRendererVideoPlayer* _video = new pmWarpPiRendererVideoPlayer();
#endif
        _video->setup("firstOne");
        _video->setupVideoPlayer("videos/"+initVideoFile, true);
        ofAddListener(_video->swapEvent, this, &pmWarpPiScreen::swapToVideo);
        screenRenderers.push_back(_video);
    }
    
    // HAS Image?
    ///////////////
    if(hasImage)
    {
        pmWarpPiRendererImagePlayer* _image = new pmWarpPiRendererImagePlayer();
        _image->setup("firstOne");
        _image->setupImagePlayer("test", false);
        ofAddListener(_image->swapEvent, this, &pmWarpPiScreen::swapToImage);
        if(initImageFile != " "){
            ofxOscMessage* m;
            m->setAddress("all");
            m->addStringArg("loadImage");
            m->addStringArg(initImageFile);
            m->addFloatArg(0);
            _image->updateOSC(m);
        }
        
        screenRenderers.push_back(_image);
    }
    
    testingImage->load("./app/testScreen.jpg");
    
    if(useFbo)
        screenFbo->allocate(screenSize.x,screenSize.y);
    
    
    /// HOMOGRAPHY
    originalCorners[0].set(0, 0);
    originalCorners[1].set(ofGetWidth(), 0);
    originalCorners[2].set(ofGetWidth(), ofGetHeight());
    originalCorners[3].set(0, ofGetHeight());
    
    distortedCorners[0].set(0, 0);
    distortedCorners[1].set(ofGetWidth(), 0);
    distortedCorners[2].set(ofGetWidth(), ofGetHeight());
    distortedCorners[3].set(0, ofGetHeight());
    
    loadConfigFromXML();
    
    ofRegisterMouseEvents(this);
    ofRegisterKeyEvents(this);
}

//-------------------------------------------------------------------------
void pmWarpPiScreen::update()
{
    /// HOMOGRAPHY
    if(doHomography)
    {
        //mouse interaction? distortedCorners[currentQuadPoint].set(ofGetMouseX(), ofGetMouseY());
        if(doEditQuadPoints) *homography = ofxHomography::findHomography(originalCorners, distortedCorners);
    }
}

//-------------------------------------------------------------------------
void pmWarpPiScreen::draw()
{
    /// HOMOGRAPHY START
    ofPushMatrix();
    if(doHomography && !isDebugging) glMultMatrixf(homography->getPtr());
    
    ofSetColor(255);
    if(useFbo) screenFbo->begin();
    
    if(isTesting){
        if(isDebugging) testingImage->draw(elementDebugRect);
        else testingImage->draw(elementRect);
    }
    else{
        if(isDebugging){
            for(int i=0;i<screenRenderers.size();i++)
            {
                screenRenderers[i]->drawElement(elementDebugRect);
            }
        }
        else{
            for(int i=0;i<screenRenderers.size();i++)
            {
                screenRenderers[i]->drawElement(elementRect);
            }
        }
    }
    
    if(useFbo){
        screenFbo->end();
        screenFbo->draw(0,0);
    }
    
    /// HOMOGRAPHY END
    ofPopMatrix();
    
    if(doEditQuadPoints && !isDebugging)
    {
        // draw circle arround corners
        for(int i=0;i<4;i++)
        {
            ofSetCircleResolution(64);
            ofNoFill();
            ofSetLineWidth(10);
            if(currentQuadPoint==i) ofSetColor(255,128,0,128);
            else ofSetColor(0,128,255,128);
            ofDrawCircle(distortedCorners[i],75);
        }
        
        // draw lines
        ofSetColor(0,128,255,128);
        ofSetLineWidth(2);
        for(int i=0;i<3;i++)
        {
            ofDrawLine(distortedCorners[i],distortedCorners[i+1]);
        }
        ofDrawLine(distortedCorners[3],distortedCorners[0]);
    }
    
    //debuging
    if(isDebugging){
        showDebug();
        for(int i=0;i<screenRenderers.size();i++)
        {
            screenRenderers[i]->showDebug();
        }
    }
}

//-------------------------------------------------------------------------
void pmWarpPiScreen::updateOSC(ofxOscMessage* m)
{
    ofLog(OF_LOG_NOTICE) << "RendScreen::updateOSC";
    string address = m->getAddress();
    
    // get the id
    string addressWithoutSlash = address.substr(1,address.size()-1);
    
    if(address=="/all")
    {
        /// THIS MESSAGE IF FOR YOU !!
        
        /// COMMAND
        string command = m->getArgAsString(0);
        
        /// TEST
        if(command == "test")
        {
            isTesting = m->getArgAsBool(1);
        }
        
        /// PLAY
        else if(command == "homography")
        {
            doHomography =  m->getArgAsBool(1);
        }
        else if(command == "fbo")
        {
            useFbo = m->getArgAsBool(1);
            if(useFbo){
                if(!screenFbo->isAllocated())
                    screenFbo->allocate(screenSize.x,screenSize.y);
            }
        }
        
        /// EDIT QUAD
        else if(command == "editQuad")
        {
            cout << "Screen received a doEditQuad !! " << endl;
            
            int val = m->getArgAsInt32(1);
            doEditQuadPoints = (val == 0) ? false : true;
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
    
    for(int i=0;i<screenRenderers.size();i++)
    {
        screenRenderers[i]->updateOSC(m);
    }
}


//-------------------------------------------------------------------------
void pmWarpPiScreen::showDebug()
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
void pmWarpPiScreen::nextQuadPoint()
{
    currentQuadPoint = (currentQuadPoint + 1 ) % 4 ;
    
    
#ifdef TARGET_OSX
    
    CGPoint point;
    point.x = ofGetWindowPositionX() + distortedCorners[currentQuadPoint].x ;
    point.y = ofGetWindowPositionY() + distortedCorners[currentQuadPoint].y ;
    
    ofLog(OF_LOG_NOTICE) <<"screen :: nextQuadX :  " << ofGetWindowPositionX() + distortedCorners[currentQuadPoint].x << " :: " << ofGetWindowPositionX() << "," <<  distortedCorners[currentQuadPoint].x;
    ofLog(OF_LOG_NOTICE) <<"screen :: nextQuadY :  " << ofGetWindowPositionY() + distortedCorners[currentQuadPoint].y<< " :: " << ofGetWindowPositionY() << "," <<  distortedCorners[currentQuadPoint].y;
    ofLog(OF_LOG_NOTICE) << ".........." ;
    CGSetLocalEventsSuppressionInterval(0);
    CGWarpMouseCursorPosition(point);
    
    //Que és??
    //ofNotifyMouseMoved(distortedCorners[currentQuadPoint].x,distortedCorners[currentQuadPoint].y);
    
    
#endif
    /*
     ofMouseEventArgs *a = new ofMouseEventArgs();
     a->set(point.x,point.y);
     a->normalize();
     mouseMoved(*a);
     */
}
//-------------------------------------------------------------------------
void pmWarpPiScreen::previousQuadPoint()
{
    currentQuadPoint = currentQuadPoint - 1 ;
    if (currentQuadPoint<0) currentQuadPoint = 3;
    
#ifdef TARGET_OSX
    CGPoint point;
    point.x = ofGetWindowPositionX() + distortedCorners[currentQuadPoint].x ;
    point.y = ofGetWindowPositionY() + distortedCorners[currentQuadPoint].y ;
    
    ofLog(OF_LOG_NOTICE) <<"screen :: nextQuadX :  " << ofGetWindowPositionX() + distortedCorners[currentQuadPoint].x << " :: " << ofGetWindowPositionX() << "," <<  distortedCorners[currentQuadPoint].x;
    ofLog(OF_LOG_NOTICE) <<"screen :: nextQuadY :  " << ofGetWindowPositionY() + distortedCorners[currentQuadPoint].y<< " :: " << ofGetWindowPositionY() << "," <<  distortedCorners[currentQuadPoint].y;
    ofLog(OF_LOG_NOTICE) << "..........";
    CGSetLocalEventsSuppressionInterval(0);
    CGWarpMouseCursorPosition(point);
    
    //Que es??
    //ofNotifyMouseMoved(distortedCorners[currentQuadPoint].x,distortedCorners[currentQuadPoint].y);
#endif
    
}

//-------------------------------------------------------------------------
void pmWarpPiScreen::resetQuad()
{
    distortedCorners[0].x = 0.0;
    distortedCorners[0].y = 0.0;
    
    distortedCorners[1].x = ofGetWidth();
    distortedCorners[1].y = 0.0;
    
    distortedCorners[2].x = ofGetWidth();
    distortedCorners[2].y = ofGetHeight();
    
    distortedCorners[3].x = 0.0;
    distortedCorners[3].y = ofGetHeight();
    
}

//-------------------------------------------------------------------------
void pmWarpPiScreen::saveConfigToXML()
{
    ofxXmlSettings configXML;
    
    configXML.clear();
    
    configXML.addTag("configQuad");
    configXML.pushTag("configQuad");
    for(int i=0;i<4;i++)
    {
        configXML.addTag("vertex");
        configXML.pushTag("vertex",i);
        
        configXML.addValue("x",distortedCorners[i].x);
        configXML.addValue("y",distortedCorners[i].y);
        configXML.popTag(); // vertex
    }
    configXML.popTag(); // configQuad
    
    ofLog(OF_LOG_NOTICE, "Saving screen info to XML : ./presets/screenConfig.xml");
    
    configXML.saveFile("./presets/screenConfig.xml");
}
//-------------------------------------------------------------------------
void pmWarpPiScreen::loadConfigFromXML()
{
    doEditQuadPoints = false;
    
    ofxXmlSettings configXML;
    configXML.clear();
    
    configXML.loadFile("./presets/screenConfig.xml");
    configXML.pushTag("configQuad");
    for(int i=0;i<4;i++)
    {
        configXML.pushTag("vertex",i);
        distortedCorners[i].x = configXML.getValue("x",0);
        distortedCorners[i].y = configXML.getValue("y",0);
        configXML.popTag(); // vertex
    }
    configXML.popTag(); // configQuad
    
    *homography = ofxHomography::findHomography(originalCorners, distortedCorners);
    
    ofLog(OF_LOG_NOTICE, "Saving screen info to XML : ./presets/screenConfig.xml");
    configXML.saveFile("./presets/screenConfig.xml");
}

//--------------------------------------------------------------
void pmWarpPiScreen::swapToImage(float &fade)
{
    if(screenRenderers[0]->getType() != renderImage){
        ((pmWarpPiRendererVideoPlayer*)screenRenderers[0])->stopVideoPlayer(fade);
        swap(screenRenderers[0], screenRenderers[1]);
    }
}

//--------------------------------------------------------------
void pmWarpPiScreen::swapToVideo(float &fade)
{
    if(screenRenderers[0]->getType() != renderVideo){
        ((pmWarpPiRendererImagePlayer*)screenRenderers[0])->stopImagePlayer(fade);
        swap(screenRenderers[0], screenRenderers[1]);
    }
}

//-------------------------------------------------------------------------
void pmWarpPiScreen::keyPressed(ofKeyEventArgs &a)
{
    int key = a.key;
    
    /// QUAD WARPING
    ////////////////
    int quadStep = 1;
    if(ofGetKeyPressed(OF_KEY_SHIFT)) quadStep = 5;
    
    
    if(key=='q')
    {
        nextQuadPoint();
    }
    else if(key=='w')
    {
        previousQuadPoint();
    }
    else if (key=='s')
    {
        saveConfigToXML();
    }
    else if (key=='l')
    {
        loadConfigFromXML();
    }
    else if (key=='d')
    {
        //isDebugging = !isDebugging;
    }
    else if (key=='t')
    {
        isTesting = !isTesting;
    }
    //    else if (key=='p')
    //    {
    //        #ifdef TARGET_RASPBERRY_PI
    //            pmWarpPiRendererOMXPlayer* vp = (pmWarpPiRendererOMXPlayer*) renderers[0];
    //            vp->setPlayerPaused(false);
    //        #else
    //            pmWarpPiRendererVideoPlayer* vp = (pmWarpPiRendererVideoPlayer*) renderers[0];
    //            vp->setPlayerPaused(false);
    //        #endif
    //    }
    else if (key=='h')
    {
        doHomography = !doHomography;
    }
    else if (key=='e')
    {
        doEditQuadPoints = !doEditQuadPoints;
    }
    
    else if (key == OF_KEY_LEFT)
    {
        distortedCorners[currentQuadPoint].x = distortedCorners[currentQuadPoint].x - quadStep;
    }
    else if (key == OF_KEY_RIGHT)
    {
        distortedCorners[currentQuadPoint].x = distortedCorners[currentQuadPoint].x + quadStep;
    }
    else if (key == OF_KEY_UP)
    {
        distortedCorners[currentQuadPoint].y = distortedCorners[currentQuadPoint].y - quadStep;
    }
    else if (key == OF_KEY_DOWN)
    {
        distortedCorners[currentQuadPoint].y = distortedCorners[currentQuadPoint].y + quadStep;
    }
    
}



