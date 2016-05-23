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

void pmWarpPiRendererArtNet::setupArtNet(string _name, bool active)
{
    pmWarpPiRendererVideoPlayer::setupVideoPlayer(_name, active);
    setUniverses(getPlayerHeight());
    bStarted = false;
}

void pmWarpPiRendererArtNet::updateForScreen()
{
    pmWarpPiRendererVideoPlayer::updateForScreen();
    sendDmx(getVideoPixels());
}

void pmWarpPiRendererArtNet::start(){
    artnet.start();
    bStarted = true;
}

void pmWarpPiRendererArtNet::setFromPixels(ofPixels &pixels){
    for (int i = 0; i < pixels.getHeight(); i++){
        for ( int j = 0; j < pixels.getLine(i).getStride()-1; j++){
            linepixels[j] = (pixels.getLine(i).asPixels()[j]);
        }
        dmxDataPacket[i].setData(linepixels);
        //dmxDataPacket[i].setPort(i);
    }
}

bool pmWarpPiRendererArtNet::sendDmx(){
    for (auto dmxUniverse : dmxDataPacket){
        artnet.sendDmx(dmxUniverse);
        cout<<dmxUniverse.getIp()<<endl;
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