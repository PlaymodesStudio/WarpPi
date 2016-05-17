//
//  pmWarpPiRendererArtNet.cpp
//  WarpPi
//
//  Created by Eduard Frigola on 04/05/16.
//
//

#include "pmWarpPiRendererArtNet.h"

bool pmWarpPiRendererArtNet::setMachineIP(string machineIP){
    
    artnet.init(machineIP);
    if(function == PM_ARTNET_PLAYER){
        artnet.setSubNet(0);
        artnet.setPortType(0, ARTNET_PORT_ENABLE_INPUT, ARTNET_DATA_DMX);
        artnet.setPortAddress(0, ARTNET_PORT_INPUT, 0);
        
    }else if(function == PM_ARTNET_RECORDER){
        artnet.setShortName("Artnet receive");
        artnet.setLongName("Artnet Mac receiver");
        artnet.setNodeType(ARTNET_TYPE_NODE);
        artnet.setSubNet(0);
        artnet.setPortType(0, ARTNET_PORT_ENABLE_OUTPUT, ARTNET_DATA_DMX);
        artnet.setPortAddress(0, ARTNET_PORT_OUTPUT, 0);
        ofAddListener(artnet.dmxData, this, &pmWarpPiRendererArtNet::receiveData);
    }
    
    ofAddListener(artnet.pollReply, this, &pmWarpPiRendererArtNet::receivePollReply);
    artnet.sendPoll();
    start();
}

bool pmWarpPiRendererArtNet::setup(pmArtnetFunction _function)
{
    function = _function;
    bStarted = false;
    //    if(function == PM_ARTNET_PLAYER){
    //        artnet.setSubNet(0);
    //        artnet.setPortType(0, ARTNET_PORT_ENABLE_INPUT, ARTNET_DATA_DMX);
    //        artnet.setPortAddress(0, ARTNET_PORT_INPUT, 0);
    //
    //    }else{
    //        artnet.setShortName("Artnet receive");
    //        artnet.setLongName("Artnet Mac receiver");
    //        artnet.setNodeType(ARTNET_TYPE_NODE);
    //        artnet.setSubNet(0);
    //        artnet.setPortType(0, ARTNET_PORT_ENABLE_OUTPUT, ARTNET_DATA_DMX);
    //        artnet.setPortAddress(0, ARTNET_PORT_OUTPUT, 0);
    //        ofAddListener(artnet.dmxData, this, &pmWarpPiRendererArtNet::receiveData);
    //    }
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