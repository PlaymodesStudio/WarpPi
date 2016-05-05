#include "pmWarpPiRendererImagePlayer.h"


//-------------------------------------------------------------------------
pmWarpPiRendererImagePlayer::pmWarpPiRendererImagePlayer()
{
    folderName = "";
    imagePosition = ofVec2f(0,0);
    imageSize = ofVec2f(0,0);
    imagePlayerDebugPosition = ofVec2f(770,20);
    
    //videoPlayer = new ofVideoPlayer();
    
    cout << "new Image Player " << endl;
    type = renderImage;
    
}
//-------------------------------------------------------------------------
void pmWarpPiRendererImagePlayer::setupImagePlayer(string _name,ofVec2f _pos, ofVec2f _size)
{
    //SET PARAMS
    folderName = _name;
    imagePosition = _pos;
    imageSize = _size;
    isTesting=false;
    folderPlay=true;
    
    //load images to vector;
    hasMedia = loadImages();
    if(hasMedia){
        currentImage=0;
        nextImage = currentImage+1;
        nextImage %= imagesInFolderPaths.size();
        images.push_back(ofImage(imagesInFolderPaths[currentImage]));
        images.push_back(ofImage(imagesInFolderPaths[nextImage]));
    }else
        images.push_back(ofImage("images/wallpaper.jpg"));
    
    //to use with time
    beginImageTime = ofGetElapsedTimef();  // get the start time
    nextImageTime = 5; // in seconds
    fadeTime = 1;
    canSwap = false;
    
    
    /// GUI
    gui->setup(); // most of the time you don't need a name but don't forget to call setup
    gui->add(screenOpacity.set( "opacity", 0.0, 0.0, 1.0));
    gui->setPosition(imagePlayerDebugPosition.x,imagePlayerDebugPosition.y + 75);
    crossFadeAlpha = screenOpacity;
    
    
//    Tweenzor::add(&crossFadeAlpha, screenOpacity, 0.0, nextImageTime-fadeTime, fadeTime,EASE_IN_OUT_EXPO);
//    Tweenzor::addCompleteListener(Tweenzor::getTween((float*)&crossFadeAlpha), this, &pmWarpPiRendererImagePlayer::onCrossFadeComplete);
    
}

//-------------------------------------------------------------------------
void pmWarpPiRendererImagePlayer::updateForScreen()
{
    
    ofLog(OF_LOG_NOTICE) << "RendVideoPlayer::update";
    
    /// TWEENZOR
    Tweenzor::update( ofGetElapsedTimeMillis() );
    
}

//-------------------------------------------------------------------------
void pmWarpPiRendererImagePlayer::drawElement(ofRectangle container)
{
    ofLog(OF_LOG_NOTICE) << "RendImagePlayer::draw";
    
    // SCREEN BACKGROUND
    ofSetColor(0,0,0,255);
    ofFill();
    //ofDrawRectangle(0,0,screenSize.x,screenSize.y);
    
    
    // DRAW Image
    
    //draw current image
    drawImage(0, crossFadeAlpha, container);
    //draw next image
    drawImage(1, screenOpacity.get()-crossFadeAlpha, container);
    
    //for testing image rotation
    if(isDebugging){
        int testSize = 100;
        drawImage(0, 255, ofRectangle(0,ofGetHeight()-testSize, testSize , testSize));
        drawImage(1, 255, ofRectangle(testSize,ofGetHeight()-testSize,testSize , testSize));
    }
}

//-----------------------------------------------------------------------
void pmWarpPiRendererImagePlayer::drawImage(int index, float alpha, ofRectangle container)
{
    ofPushStyle();
    ofSetColor(255, 255, 255, alpha*255);
    //if(isDebugging)
    //images[index].draw(0,ofGetHeight()-ofGetHeight()/2,ofGetWidth()/2,ofGetHeight()/2);
    //else
    images[index].draw(container);
    ofPopStyle();
}


//-------------------------------------------------------------------------
void pmWarpPiRendererImagePlayer::updateOSC(ofxOscMessage* m)
{
    ofLog(OF_LOG_NOTICE) << "RendVideoPlayer::updateOSC";
    
    
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
            //no folder play
            //get varliables
            auto temp_fadeTime = m->getArgAsFloat(2);
            
            
//            images.pop_back();
//            images.push_back(ofImage("images/"+imagePath));
            
//            bool toSend = true;
            //ofNotifyEvent(swapEvent, toSend, this);
            
            Tweenzor::add(&crossFadeAlpha, screenOpacity, 0.0, 0.0, temp_fadeTime, EASE_IN_OUT_EXPO);
            Tweenzor::addCompleteListener(Tweenzor::getTween(&crossFadeAlpha), this, &pmWarpPiRendererImagePlayer::onCrossFadeComplete);
            canSwap = false;

        }
        
        /// STOP
        if(command == "stop")
        {
            //no folder play
            folderPlay = false;
            //get varliables
            auto temp_fadeTime = m->getArgAsFloat(1);
            
            images.pop_back();
            images.push_back(ofImage(" "));

            bool toSend = true;
            //ofNotifyEvent(swapEvent, toSend, this);
            
            Tweenzor::add(&crossFadeAlpha, screenOpacity, 0.0, 0.0, temp_fadeTime, EASE_IN_OUT_EXPO);
            //Tweenzor::addCompleteListener(Tweenzor::getTween(&crossFadeAlpha), this, &pmWarpPiRendererImagePlayer::onCrossFadeComplete);
        }
        
        /// LOAD IMAGE
        if(command == "loadImage")
        {
            if(canSwap){
                //no folder play
                folderPlay = false;
                //get varliables
                auto temp_imagePath = m->getArgAsString(1);
                fadeTime = m->getArgAsFloat(2);
                
                images.pop_back();
                images.push_back(ofImage("images/"+temp_imagePath));
                if(images[1].getWidth() != 0)
                    imagePath = temp_imagePath;
                
                bool toSend = true;
                ofNotifyEvent(swapEvent, toSend, this);
                
                Tweenzor::add(&crossFadeAlpha, screenOpacity, 0.0, 0.0, fadeTime, EASE_IN_OUT_EXPO);
                Tweenzor::addCompleteListener(Tweenzor::getTween(&crossFadeAlpha), this, &pmWarpPiRendererImagePlayer::onCrossFadeComplete);
                canSwap = false;
            }
        }
        else if(command == "loadFolder")
        {
            folderPlay = true;
            folderName = m->getArgAsString(1);
            fadeTime = m->getArgAsFloat(2);
            hasMedia = loadImages();
            if(hasMedia){
                currentImage=-1;
                nextImage = currentImage+1;
                nextImage %= imagesInFolderPaths.size();
                images.pop_back();
                images.push_back(ofImage(imagesInFolderPaths[nextImage]));
            }
            
            
            Tweenzor::add(&crossFadeAlpha, screenOpacity, 0.0, 0.0, fadeTime,EASE_IN_OUT_EXPO);
            Tweenzor::addCompleteListener(Tweenzor::getTween(&crossFadeAlpha), this, &pmWarpPiRendererImagePlayer::onCrossFadeComplete);
            canSwap = false;
        }
    }
    
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

//--------------------------------------------------------------
void pmWarpPiRendererImagePlayer::onCrossFadeComplete(float *arg)
{
    cout<<"tween completed"<<endl;
    if(folderPlay){
        currentImage++;
        currentImage %= imagesInFolderPaths.size();
        nextImage = currentImage+1;
        nextImage %= imagesInFolderPaths.size();
        images.pop_front();
        images.push_back(ofImage(imagesInFolderPaths[nextImage]));
        crossFadeAlpha=screenOpacity;
        
        Tweenzor::add(&crossFadeAlpha, screenOpacity, 0.0, nextImageTime-fadeTime, fadeTime,EASE_IN_OUT_EXPO);
        Tweenzor::addCompleteListener(Tweenzor::getTween(&crossFadeAlpha), this, &pmWarpPiRendererImagePlayer::onCrossFadeComplete);
    }
    else
    {
        images.pop_front();
        images.push_back(ofImage("images/"+imagePath));
        crossFadeAlpha=screenOpacity;
    }
    canSwap = true;
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
    if(currentImage != -1 && folderPlay)
        ofDrawBitmapString(imagesInFolderPaths[currentImage],imagePlayerDebugPosition.x,whichHeight);
    else
        ofDrawBitmapString(imagePath, imagePlayerDebugPosition.x,whichHeight);
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
    imagesInFolderPaths.clear();
    // you can now iterate through the files and load them into the ofImage vector
    for(int i = 0; i < (int)dir.size(); i++){
        imagesInFolderPaths.push_back(dir.getPath(i));
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
