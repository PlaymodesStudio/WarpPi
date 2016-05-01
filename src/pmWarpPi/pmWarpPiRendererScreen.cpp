#include "pmWarpPiRendererScreen.h"

//-------------------------------------------------------------------------
pmWarpPiRendererScreen::pmWarpPiRendererScreen()
{
    screenRect = ofRectangle(0,0, ofGetWidth(), ofGetHeight());
    elementRect = screenRect;
    elementDebugRect = ofRectangle(0,ofGetHeight()-ofGetHeight()/2,ofGetWidth()/2,ofGetHeight()/2);
    
    screenPosition = ofVec2f(0,0);
    screenSize = ofVec2f(0,0);
    doHomography = true;
    useFbo = false;
    doEditQuadPoints = false;
    shiftPressed = false;
    isTesting = false;
    isDebugging= false;
    currentQuadPoint = 0;
    screenDebugPosition = ofVec2f(270,20);
    testingImage = new ofImage();
    homography = new ofMatrix4x4();
    screenFbo = new ofFbo();
    screenOpacity = 1.0;
    maxScreenOpacity = 1.0;
    
    cout << "pmRendererScreen :: new reder screen";
}

//-------------------------------------------------------------------------
void pmWarpPiRendererScreen::setupScreen(ofVec2f _pos,ofVec2f _size)
{
    screenRect.setPosition(_pos);
    screenRect.setSize(_size.x, _size.y);
    screenPosition = _pos;
    screenSize = _size;
    doHomography = true;
    doEditQuadPoints = false;
    isTesting = false;
    currentQuadPoint = 0;

    testingImage->load("./app/testScreen.jpg");
    
    if(useFbo)
        screenFbo->allocate(screenSize.x,screenSize.y);
 
    /// TWEENZOR
    // must call this before adding any tweens //
	Tweenzor::init();
    
//    Tweenzor::add((float *)&screenOpacity.get(), 0.0, 1.0, 0.0, 4,EASE_IN_OUT_EXPO);
//    Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererScreen::onComplete);

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
void pmWarpPiRendererScreen::update(ofEventArgs & a)
{
    /// TWEENZOR
    Tweenzor::update( ofGetElapsedTimeMillis() );

    // all "tweened" values needs to be reassigned here !!
    screenOpacity = screenOpacity;

    //ofLog(OF_LOG_NOTICE) << "RendScreen::update";

    updateForScreen();
    
    /// HOMOGRAPHY
    if(doHomography)
    {
        //mouse interaction? distortedCorners[currentQuadPoint].set(ofGetMouseX(), ofGetMouseY());
        
        if(doEditQuadPoints) *homography = ofxHomography::findHomography(originalCorners, distortedCorners);
        
        
    }

}

//-------------------------------------------------------------------------
void pmWarpPiRendererScreen::draw()
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
        if(isDebugging) drawElement(elementDebugRect);
        else drawElement(elementRect);
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
    
    if(isDebugging) showScreenDebug(); showDebug();
    
    gui->draw();
}


//-------------------------------------------------------------------------
void pmWarpPiRendererScreen::updateOSC(ofxOscMessage* m)
{
    ofLog(OF_LOG_NOTICE) << "RendScreen::updateOSC";
    string address = m->getAddress();
    
    // get the id
    string addressWithoutSlash = address.substr(1,address.size()-1);
    
    if((address=="/all")||(id==addressWithoutSlash))
    {
        /// THIS MESSAGE IF FOR YOU !!
        
        /// COMMAND
        string command = m->getArgAsString(0);
        
        /// PLAY
        if(command == "homography")
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
    }
}

//-------------------------------------------------------------------------
void pmWarpPiRendererScreen::deleteRenderer()
{
    ofLog(OF_LOG_NOTICE) << "RendScreen::delete";
    Tweenzor::destroy();

}

//-------------------------------------------------------------------------
void pmWarpPiRendererScreen::testScreen()
{
    if(useFbo)
        screenFbo->begin();
    
    ofSetColor(255,128,0);
    ofFill();
    ofDrawRectangle(screenPosition.x,screenPosition.y,screenSize.x,screenSize.y);
    ofSetColor(255);
    testingImage->draw(screenPosition.x,screenPosition.y,screenSize.x,screenSize.y);
    
    if(useFbo);
        screenFbo->end();
}

//-------------------------------------------------------------------------
void pmWarpPiRendererScreen::showScreenDebug()
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
void pmWarpPiRendererScreen::nextQuadPoint()
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
    
    //Que Žs??
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
void pmWarpPiRendererScreen::previousQuadPoint()
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
void pmWarpPiRendererScreen::resetQuad()
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
void pmWarpPiRendererScreen::keyPressed(ofKeyEventArgs &a)
{
    if(a.key == OF_KEY_SHIFT){
        shiftPressed=true;
    }
    int key = a.key;
    
    int quadStep = 1;
    if(ofGetKeyPressed(OF_KEY_SHIFT)) quadStep = 1;
    
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

    /// QUAD WARPING
    ////////////////
    
    else if (key == OF_KEY_LEFT)
    {
        distortedCorners[currentQuadPoint].x = distortedCorners[currentQuadPoint].x - (quadStep * ((shiftPressed) ? 5 : 1));
    }
    else if (key == OF_KEY_RIGHT)
    {
        distortedCorners[currentQuadPoint].x = distortedCorners[currentQuadPoint].x + (quadStep * ((shiftPressed) ? 5 : 1));
    }
    else if (key == OF_KEY_UP)
    {
        distortedCorners[currentQuadPoint].y = distortedCorners[currentQuadPoint].y - (quadStep * ((shiftPressed) ? 5 : 1));
    }
    else if (key == OF_KEY_DOWN)
    {
        distortedCorners[currentQuadPoint].y = distortedCorners[currentQuadPoint].y + (quadStep * ((shiftPressed) ? 5 : 1));
    }
    
    
    
}
//-------------------------------------------------------------------------
void pmWarpPiRendererScreen::keyReleased(ofKeyEventArgs &a)
{
    if(a.key == OF_KEY_SHIFT){
        shiftPressed=false;
    }
}
//-------------------------------------------------------------------------
void pmWarpPiRendererScreen::mouseMoved(ofMouseEventArgs &a)
{

}
//-------------------------------------------------------------------------
void pmWarpPiRendererScreen::mouseDragged(ofMouseEventArgs &a)
{
    
}
//-------------------------------------------------------------------------
void pmWarpPiRendererScreen::mousePressed(ofMouseEventArgs &a)
{
    
}
//-------------------------------------------------------------------------
void pmWarpPiRendererScreen::mouseReleased(ofMouseEventArgs &a)
{
    
}
//-------------------------------------------------------------------------
void pmWarpPiRendererScreen::saveConfigToXML()
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
void pmWarpPiRendererScreen::loadConfigFromXML()
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
