//
//  pmWarpPiRendererArtNet.cpp
//  WarpPi
//
//  Created by Eduard Frigola on 04/05/16.
//
//

#include "pmWarpPiRendererArtNet.h"

pmWarpPiRendererArtNet::pmWarpPiRendererArtNet()
{
    nUniverses = -1;
}

bool pmWarpPiRendererArtNet::setMachineIP(string machineIP)
{
    artnet.init(machineIP);
    artnet.setSubNet(0);
    artnet.setPortType(0, ARTNET_PORT_ENABLE_INPUT, ARTNET_DATA_DMX);
    artnet.setPortAddress(0, ARTNET_PORT_INPUT, 0);

    
    ofAddListener(artnet.pollReply, this, &pmWarpPiRendererArtNet::receivePollReply);
    artnet.sendPoll();
    start();
}

void pmWarpPiRendererArtNet::setupArtNet(string _name, string machineIP, string targetIP, int subnet, int universe, bool active)
{
    pmWarpPiRendererVideoPlayer::setupVideoPlayer(_name, active);
//    setUniverses(getPlayerHeight());
    setUniverses(1);
    bStarted = false;
    setMachineIP(machineIP);
    for(int i=0; i<nUniverses; i++){
        setTargetIP(targetIP, i);
        setTargetSubNet(subnet, i);
        setTargetUniverse(universe+i, i);
    }
}

void pmWarpPiRendererArtNet::updateForScreen()
{
    pmWarpPiRendererVideoPlayer::updateForScreen();
    if(!isPlayerPaused())
        sendDmx(getVideoPixels());
}

void pmWarpPiRendererArtNet::updateOSC(ofxOscMessage *m)
{
    //translate artnetplay to videoplay
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
        ofxOscMessage *mTemp;
        mTemp = new ofxOscMessage();
        mTemp->setAddress(m->getAddress());
        mTemp->addStringArg("loadVideo");
        mTemp->addStringArg(m->getArgAsString(1));
        mTemp->addFloatArg(m->getArgAsFloat(2));
        pmWarpPiRendererVideoPlayer::updateOSC(mTemp);
//        setUniverses(getPlayerHeight());
        /// SET ONE DMX CHANNEL
    }else if(command == "setDmxCh"){
        int whichCh         = m->getArgAsInt32(1)-1;
        if((whichCh>=0) && (whichCh<512))
        {
            int dmxValue        = m->getArgAsInt32(2);
            //float timeToValue   = m->getArgAsFloat(3);
    
            linepixels[whichCh] = dmxValue;
            sendDmx();
        }
    }else if(command == "loopArtnet"){
        m2->addStringArg("loop");
        m2->addBoolArg(m->getArgAsBool(1));
}

    pmWarpPiRendererVideoPlayer::updateOSC(m2);
}

void pmWarpPiRendererArtNet::start(){
    artnet.start();
    bStarted = true;
}

void pmWarpPiRendererArtNet::setFromPixels(ofPixels &pixels){
//    for (int i = 0; i < pixels.getHeight(); i++){
        for ( int j = 0; j < pixels.getLine(0).getStride()-1; j++){
            linepixels[j] = (pixels.getLine(0).asPixels()[j]);
        }
        dmxDataPacket[0].setData(linepixels);
        //dmxDataPacket[i].setPort(i);
//    }
}

bool pmWarpPiRendererArtNet::sendDmx(){
    for (auto dmxUniverse : dmxDataPacket){
        vector<unsigned char> scaledData;
        scaledData.resize(dmxUniverse.getData().size());
        for(int i = 0; i < scaledData.size() ; i++)
            scaledData[i] =(unsigned char)((float)dmxUniverse.getData()[i]*screenOpacity);
        
        dmxUniverse.setData(scaledData);
        artnet.sendDmx(dmxUniverse);
        //cout<<dmxUniverse.getIp()<<endl;
    }
}

bool pmWarpPiRendererArtNet::sendDmx(ofPixels &pixels){
    setFromPixels(pixels);
    sendDmx();
}

void pmWarpPiRendererArtNet::setUniverses(int universes){
    nUniverses = universes;
    for(int i = 0 ; i<nUniverses ; i++){
        ofxArtNetDmxData tempPacket;
        tempPacket.setPort(0);
        vector<unsigned char> dataVec;
        dataVec.assign(512, sizeof(unsigned char));
        tempPacket.setData(dataVec);
        dmxDataPacket.push_back(tempPacket);
    }
    linepixels.assign(512, sizeof(unsigned char));
}

void pmWarpPiRendererArtNet::receivePollReply(ofxArtNetNodeEntry &node){
    string ipOfNode = node.getIp();
    ofNotifyEvent(receivedNode,ipOfNode);
    int univi = node.getUniverseOutput(0);
    univi = univi>>14;
    cout<<"NODE FOUND: "<<node.getIp()<< "  -  Subnet "<<node.getSubnet()<<"  Universe "<<node.getShortName()<<endl;
}