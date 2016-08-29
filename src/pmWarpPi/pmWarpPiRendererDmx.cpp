//
//  pmWarpPiRendererDmx.cpp
//  WarpPi
//
//  Created by Eduard Frigola on 04/05/16.
//
//

#include "pmWarpPiRendererDmx.h"

pmWarpPiRendererDmx::pmWarpPiRendererDmx()
{
    dmx = new ofxDmx();
    linepixels.resize(512, 0);
//    linepixels.assign(512, sizeof(unsigned char));
}

void pmWarpPiRendererDmx::setupDmx(string _name, int _numDevice, bool active)
{
    pmWarpPiRendererVideoPlayer::setupVideoPlayer(_name, active);
    bStarted = false;
    numDevice = _numDevice;
    start();
}

void pmWarpPiRendererDmx::updateForScreen()
{
    pmWarpPiRendererVideoPlayer::updateForScreen();
    if(!isPlayerPaused())
        sendDmx(getVideoPixels());
}

void pmWarpPiRendererDmx::updateOSC(ofxOscMessage *m)
{
    //translate Dmxplay to videoplay
    ofxOscMessage *m2;
    m2 = new ofxOscMessage();
    m2->setAddress(m->getAddress());
    string command = m->getArgAsString(0);
    if(command == "playArtnet"){
        m2->addStringArg("playVideo");
        m2->addFloatArg(m->getArgAsFloat(1));
    }
    else if(command == "stopArtnet"){
        m2->addStringArg("stopVideo");
        m2->addFloatArg(m->getArgAsFloat(1));
    }
    else if(command == "pauseArtnet"){
        m2->addStringArg("pauseVideo");
    }
    else if(command == "restartArtnet"){
        m2->addStringArg("restartVideo");
        m2->addFloatArg(m->getArgAsFloat(1));
    }
    else if(command == "loadArtnet"){
        m2->addStringArg("loadVideo");
        m2->addStringArg(m->getArgAsString(1));
        m2->addFloatArg(m->getArgAsFloat(2));
        /// SET ONE DMX CHANNEL
    }else if(command == "setDmxCh"){
        int whichCh         = m->getArgAsInt32(1)-1;
        if((whichCh>=0) && (whichCh<512))
        {
            int dmxValue        = m->getArgAsInt32(2);
            //float timeToValue   = m->getArgAsFloat(3);
            
            ofPixels tempPix;
            tempPix.allocate(171, 1, 3);
            for(int i = 0; i<whichCh; i++){
                tempPix[i] = 0;
            }
            linepixels[whichCh] = dmxValue;
            sendDmx();
        }
    }else if(command == "loopArtnet"){
        m2->addStringArg("loop");
        m2->addBoolArg(m->getArgAsBool(1));
}

    pmWarpPiRendererVideoPlayer::updateOSC(m2);
}

void pmWarpPiRendererDmx::start(){
    dmx->connect("/dev/cu.usbserial-ENS2NSW5"); // use the name
//    dmx->connect(numDevice); // or use a number
    bStarted = true;
}

void pmWarpPiRendererDmx::setFromPixels(ofPixels &pixels){
    for(int j = 0; j < pixels.getLine(0).getStride()-1; j++){
        linepixels[j] = (pixels.getLine(0).asPixels()[j]);
    }
}

bool pmWarpPiRendererDmx::sendDmx(){
    for(int i=0;i<linepixels.size();i++)
    {
        dmx->setLevel(i+1,linepixels[i]*screenOpacity);
        cout<<ofToInt(ofToString(linepixels[i]))<< " - ";
    }
    cout<<endl;
    dmx->update();
}

bool pmWarpPiRendererDmx::sendDmx(ofPixels &pixels){
    setFromPixels(pixels);
    sendDmx();
}