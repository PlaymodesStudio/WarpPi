#include "pmWarpPiRendererImagePlayer.h"


//-------------------------------------------------------------------------
pmWarpPiRendererImagePlayer::pmWarpPiRendererImagePlayer()
{
    folderName = "";
    elementDebugInfoPos = ofVec2f(770,20);
    
    //videoPlayer = new ofVideoPlayer();
    
    cout << "new Image Player " << endl;
    type = renderImage;
    
}
//-------------------------------------------------------------------------
void pmWarpPiRendererImagePlayer::setupImagePlayer(string _name, bool active)
{
    pmWarpPiRendererDrawable::setupScreen();
    
    //SET PARAMS
    folderName = _name;
    isTesting=false;
    folderPlay=true;
    activePlayer = active;
    
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
    
    /// GUI
    gui->setup(); // most of the time you don't need a name but don't forget to call setup
    gui->add(screenOpacity.set( "opacity", 0.0, 0.0, 1.0));
    gui->setPosition(770, 20 + 75);
    crossFadeAlpha = 1;
    
    if(activePlayer){
        Tweenzor::add(&crossFadeAlpha, screenOpacity, 0.0, nextImageTime-fadeTime, fadeTime,EASE_IN_OUT_EXPO);
        Tweenzor::addCompleteListener(Tweenzor::getTween((float*)&crossFadeAlpha), this, &pmWarpPiRendererImagePlayer::onCrossFadeComplete);
    }
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
    ofSetColor(255, 255, 255, alpha*255*screenOpacity);
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
        if(command == "playImage")
        {
            //no folder play
            //get varliables
            auto temp_fadeTime = m->getArgAsFloat(1);
            
            if(!activePlayer){
                activePlayer = true;
                ofNotifyEvent(swapEvent, temp_fadeTime, this);
                startImagePlayer(fadeTime);
            }
            
            
//            images.pop_back();
//            images.push_back(ofImage("images/"+imagePath));
            
//            bool toSend = true;
            //ofNotifyEvent(swapEvent, toSend, this);
            
            Tweenzor::add(&crossFadeAlpha, screenOpacity, 0.0, 0.0, temp_fadeTime, EASE_IN_OUT_EXPO);
            Tweenzor::addCompleteListener(Tweenzor::getTween(&crossFadeAlpha), this, &pmWarpPiRendererImagePlayer::onCrossFadeComplete);
            isFading = true;
        }
        
        /// STOP
        if(command == "stopImage")
        {

            stopImagePlayer(m->getArgAsFloat(1));
            //bool toSend = true;
            //ofNotifyEvent(swapEvent, toSend, this);
        }
        
        /// LOAD IMAGE
        if(command == "loadImage")
        {
            if(!isFading){
                //no folder play
                folderPlay = false;
                //get varliables
                auto temp_imagePath = m->getArgAsString(1);
                fadeTime = m->getArgAsFloat(2);
                
                if(!activePlayer){
                    activePlayer = true;
                    ofNotifyEvent(swapEvent, fadeTime, this);
                    startImagePlayer(fadeTime);
                }
                
                images.pop_back();
                images.push_back(ofImage("images/"+temp_imagePath));
                if(images[1].getWidth() != 0)
                    imagePath = temp_imagePath;
                
                
                Tweenzor::add(&crossFadeAlpha, screenOpacity, 0.0, 0.0, fadeTime, EASE_IN_OUT_EXPO);
                Tweenzor::addCompleteListener(Tweenzor::getTween(&crossFadeAlpha), this, &pmWarpPiRendererImagePlayer::onCrossFadeComplete);
                isFading = true;
            }
        }
        else if(command == "loadFolder")
        {
            
            folderPlay = true;
            folderName = m->getArgAsString(1);
            fadeTime = m->getArgAsFloat(2);
            nextImageTime = m->getArgAsFloat(3);
            
            if(!activePlayer){
                activePlayer = true;
                ofNotifyEvent(swapEvent, fadeTime, this);
                startImagePlayer(fadeTime);
            }
            
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
            isFading = true;
        }
    }
    pmWarpPiRendererDrawable::updateOSC(m);
}

//--------------------------------------------------------------
void pmWarpPiRendererImagePlayer::onComplete(float* arg)
{
    // this function is called on when the tween is complete //
    cout << "screen :: onComplete : arg = " << *arg << endl;
    
    if(arg == &screenOpacity.get())
    {
        cout << "this is a stop?" << endl;
    }
}

//--------------------------------------------------------------
void pmWarpPiRendererImagePlayer::onCrossFadeComplete(float *arg)
{
    cout<<"tween completed"<<endl;
    if(folderPlay && activePlayer){
        currentImage++;
        currentImage %= imagesInFolderPaths.size();
        nextImage = currentImage+1;
        nextImage %= imagesInFolderPaths.size();
        images.pop_front();
        images.push_back(ofImage(imagesInFolderPaths[nextImage]));
        crossFadeAlpha=screenOpacity;
        
        Tweenzor::add(&crossFadeAlpha, screenOpacity, 0.0, nextImageTime, fadeTime,EASE_IN_OUT_EXPO);
        Tweenzor::addCompleteListener(Tweenzor::getTween(&crossFadeAlpha), this, &pmWarpPiRendererImagePlayer::onCrossFadeComplete);
    }
    else
    {
        images.pop_front();
        images.push_back(ofImage("images/"+imagePath));
        crossFadeAlpha=screenOpacity;
    }
    isFading = false;
}

//-------------------------------------------------------------------------
void pmWarpPiRendererImagePlayer::deleteRenderer()
{
    ofLog(OF_LOG_NOTICE) << "RendVideoPlayer::delete";
    
    images.clear();
    
}

//-------------------------------------------------------------------------
void pmWarpPiRendererImagePlayer::showDebug()
{
    
    int lineHeight = 15;
    int whichHeight = elementDebugInfoPos.y;
    ofSetColor(255,128,0);
    ofDrawBitmapString("IMAGE PLAYER",elementDebugInfoPos.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofSetColor(255);
    ofDrawBitmapString(folderName,elementDebugInfoPos.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofDrawBitmapString(ofToString(images[0].getWidth()) + " x " +ofToString(images[0].getHeight()),elementDebugInfoPos.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    if(currentImage != -1 && folderPlay)
        ofDrawBitmapString(imagesInFolderPaths[currentImage],elementDebugInfoPos.x,whichHeight);
    else
        ofDrawBitmapString(imagePath, elementDebugInfoPos.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    string loopType;
    
    
}

//-------------------------------------------------------------------------
void pmWarpPiRendererImagePlayer::stopImagePlayer(float _fadeTime)
{
    activePlayer = false;
    
    images.pop_back();
    images.push_back(ofImage(" "));
    
    Tweenzor::add(&crossFadeAlpha, screenOpacity, 0.0, 0.0, _fadeTime, EASE_IN_OUT_EXPO);
    Tweenzor::addCompleteListener(Tweenzor::getTween(&crossFadeAlpha), this, &pmWarpPiRendererImagePlayer::onCrossFadeComplete);
}

//-------------------------------------------------------------------------
void pmWarpPiRendererImagePlayer::startImagePlayer(float _fadeTime)
{
    Tweenzor::add((float *)&screenOpacity.get(), 0.0, maxScreenOpacity, 0.0, _fadeTime, EASE_IN_OUT_EXPO);
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
