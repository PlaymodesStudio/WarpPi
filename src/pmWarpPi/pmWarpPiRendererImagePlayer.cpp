#include "pmWarpPiRendererImagePlayer.h"


//-------------------------------------------------------------------------
pmWarpPiRendererImagePlayer::pmWarpPiRendererImagePlayer()
{
    folderName = "";
    imagePosition = ofVec2f(0,0);
    imageSize = ofVec2f(0,0);
    imagePlayerDebugPosition = ofVec2f(520,20);
    
    //videoPlayer = new ofVideoPlayer();
    
    cout << "new Image Player " << endl;
    
}
//-------------------------------------------------------------------------
void pmWarpPiRendererImagePlayer::setupImagePlayer(string _name,ofVec2f _pos, ofVec2f _size)
{
    //SET PARAMS
    folderName = _name;
    imagePosition = _pos;
    imageSize = _size;
    isTesting=false;
    
    
    //load images to vector;
    hasMedia = loadImages();
    if(hasMedia){
        currentImage=0;
        nextImage = currentImage+1;
        nextImage %= imagePaths.size();
        images.push_back(ofImage(imagePaths[currentImage]));
        images.push_back(ofImage(imagePaths[nextImage]));
    }else
        images.push_back(ofImage("images/wallpaper.jpg"));
    
    //to use with time
    beginImageTime = ofGetElapsedTimef();  // get the start time
    nextImageTime = 5; // in seconds
    fadeTime = 1;
    

    /// GUI
    gui->setup(); // most of the time you don't need a name but don't forget to call setup
	gui->add(screenOpacity.set( "opacity", 1.0, 0.0, 1.0));
    gui->setPosition(imagePlayerDebugPosition.x,imagePlayerDebugPosition.y + 75);
    
}

//-------------------------------------------------------------------------
void pmWarpPiRendererImagePlayer::updateForScreen()
{
    
    ofLog(OF_LOG_NOTICE) << "RendVideoPlayer::update";
    
    /// TWEENZOR
    Tweenzor::update( ofGetElapsedTimeMillis() );
    screenOpacity = screenOpacity;
    
    if(ofGetElapsedTimef()-beginImageTime > nextImageTime){
        if(hasMedia){
            currentImage++;
            currentImage %= imagePaths.size();
            nextImage = currentImage+1;
            nextImage %= imagePaths.size();
            images.pop_front();
            images.push_back(ofImage(imagePaths[nextImage]));
        }
        beginImageTime = ofGetElapsedTimef();
    }
    
    
    if(isTesting)
    {
        //videoPlayer->setVolume(0);
    }
    else
    {
        //videoPlayer->setVolume(screenOpacity);
        
    }
    
    //videoPlayer->update();
}

//-------------------------------------------------------------------------
void pmWarpPiRendererImagePlayer::drawIntoFbo()
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
        //ofDrawRectangle(0,0,screenSize.x,screenSize.y);
        
        if(isTesting)
        {
            ofSetColor(255);
            testingImage->draw(imagePosition.x,imagePosition.y,imageSize.x,imageSize.y);
        }
        else
        {
            // DRAW VIDEO
            ofSetColor(255 * screenOpacity);
            int maxAlpha = screenOpacity*255;
            if(hasMedia){
                ofHideCursor();
                int alphaValue = 255;
                if(ofGetElapsedTimef()-beginImageTime > nextImageTime-(fadeTime))
                    alphaValue = maxAlpha - ((ofGetElapsedTimef()-beginImageTime)-(nextImageTime-(fadeTime)))/(fadeTime)*maxAlpha;
                
                //draw current image
                drawImage(0, alphaValue);
                
                //draw next image
                if(alphaValue != maxAlpha){
                    drawImage(1, (maxAlpha-alphaValue));
                }
            }else if(images.size() != 0)
                drawImage(0, maxAlpha);
        }
    //----
    screenFbo->end();
    //----
}

//
void pmWarpPiRendererImagePlayer::drawImage(int index, int alpha)
{
    ofPushStyle();
    ofSetColor(255, 255, 255, alpha);
    if(isDebugging)
        images[index].draw(0,ofGetHeight()-ofGetHeight()/2,ofGetWidth()/2,ofGetHeight()/2);
    else
        images[index].draw(imagePosition.x, imagePosition.y, imageSize.x, imageSize.y);
    ofPopStyle();
}


//-------------------------------------------------------------------------
void pmWarpPiRendererImagePlayer::updateOSC(ofxOscMessage* m)
{
    ofLog(OF_LOG_NOTICE) << "RendVideoPlayer::updateOSC";
    
    
//    string address = m->getAddress();
//    
//    if(address.find("play")!=-1)
//    {
//        videoPlayer->play();
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
//        if(videoPlayer->isPaused()) videoPlayer->setPaused(false);
//        else videoPlayer->setPaused(true);
//        
//    }
//    if(address.find("restart")!=-1)
//    {
//        videoPlayer->setPaused(true);
//        videoPlayer->setPosition(0.0);
//        videoPlayer->play();
//        Tweenzor::add((float *)&screenOpacity.get(), 0.0, 1.0, 0.0, m->getArgAsFloat(0),EASE_IN_OUT_EXPO);
//        Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererVideoPlayer::onComplete);
//    }
//    
}

//--------------------------------------------------------------
void pmWarpPiRendererImagePlayer::onComplete(float* arg)
{
    // this function is called on when the tween is complete //
    cout << "screen :: onComplete : arg = " << *arg << endl;
    
    if(arg == &screenOpacity.get())
    {
        cout << "this is a stop?" << endl;
        //if(screenOpacity==0.0) videoPlayer->stop();
    }
    
    //    if(arg == &_x2.get())
    //    {
    //        Tweenzor::add((float *)&_x1.get(), 0, ofGetWidth(), 0, 2,EASE_IN_OUT_EXPO);
    //        Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&_x1.get()), this, &testApp::onComplete);
    //    }
    
}

//-------------------------------------------------------------------------
void pmWarpPiRendererImagePlayer::deleteRenderer()
{
    ofLog(OF_LOG_NOTICE) << "RendVideoPlayer::delete";
    
    //videoPlayer->close();
    
}

//-------------------------------------------------------------------------
void pmWarpPiRendererImagePlayer::showDebug()
{
        
    int lineHeight = 15;
    int whichHeight = imagePlayerDebugPosition.y;
    ofSetColor(255,128,0);
    ofDrawBitmapString("IMAGE PLAYER",imagePlayerDebugPosition.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofSetColor(255);
    ofDrawBitmapString(folderName,imagePlayerDebugPosition.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofDrawBitmapString(ofToString(images[0].getWidth()) + " x " +ofToString(images[0].getHeight()),imagePlayerDebugPosition.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofDrawBitmapString(imagePaths[currentImage],imagePlayerDebugPosition.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    string loopType;
    
    
}

//-------------------------------------------------------------------------
bool pmWarpPiRendererImagePlayer::loadImages()
{
    dir.listDir("images/"+folderName);
    if(dir.size() == 0)
        return false;
    
    dir.sort(); // in linux the file system doesn't return file lists ordered in alphabetical order
    
    // you can now iterate through the files and load them into the ofImage vector
    for(int i = 0; i < (int)dir.size(); i++){
        imagePaths.push_back(dir.getPath(i));
    }
    return true;
}

//-------------------------------------------------------------------------
void pmWarpPiRendererImagePlayer::keyPressed(ofKeyEventArgs &a)
{
    int key = a.key;
    
    if(key=='p')
    {
//        videoPlayer->play();
    }
    
    cout << "videoplayer key pressed " << key << endl;
    
}
//-------------------------------------------------------------------------
void pmWarpPiRendererImagePlayer::keyReleased(ofKeyEventArgs &a)
{
    
}
//-------------------------------------------------------------------------
void pmWarpPiRendererImagePlayer::mouseMoved(ofMouseEventArgs &a)
{
}
//-------------------------------------------------------------------------
void pmWarpPiRendererImagePlayer::mouseDragged(ofMouseEventArgs &a)
{
    
}
//-------------------------------------------------------------------------
void pmWarpPiRendererImagePlayer::mousePressed(ofMouseEventArgs &a)
{
    
}
//-------------------------------------------------------------------------
void pmWarpPiRendererImagePlayer::mouseReleased(ofMouseEventArgs &a)
{
    
}
