//
//  pmWarpPiRendererArtNet.h
//  WarpPi
//
//  Created by Eduard Frigola on 04/05/16.
//
//

#ifndef pmWarpPiRendererArtNet_h
#define pmWarpPiRendererArtNet_h

#include "pmWarpPi.h"
#include "ofxArtNet.h"
#include "pmWarpPiRendererVideoPlayer.h"

class pmWarpPiRendererArtNet: public pmWarpPiRendererVideoPlayer{
    
public:
    pmWarpPiRendererArtNet();
    
    virtual void        updateOSC(ofxOscMessage* m); //implementar i que overridii els de videoplyer i cridi els play, no els parseji directe.
    virtual void        deleteRenderer(){};
    
    /// CLASS FUNCTIONS
    ////////////////////
    virtual void        updateForScreen();
    void                setupArtNet(string _name, string machineIP, string targetIP, int subnet, int universe, bool active = true);
    virtual void        showDebug(){};

    
    bool setMachineIP(string machineIP);
    void start();
    void setFromPixels(ofPixels &pixels);
    bool sendDmx();
    bool sendDmx(ofPixels &pixels);
    void setTargetIP(string ip, int line){if(nUniverses > 0) dmxDataPacket[line].setIp(ip);};
    void setTargetSubNet(int subnet, int line){dmxDataPacket[line].setSubNet(subnet);};
    void setTargetUniverse(int universe, int line){dmxDataPacket[line].setUniverse(universe);};
    vector<unsigned char> getData(int universe){return dmxDataPacket[universe].getData();};
    void receiveData(ofxArtNetDmxData &data){dmxDataPacket[data.getPort()] = data;};
    void receivePollReply(ofxArtNetNodeEntry &node);
    bool isStarted(){return bStarted;};
    vector<pair<string, string>> getIfacesIps(){return artnet.getIfacesIps();};
    
    int getUniverses(){return dmxDataPacket.size();};
    void setUniverses(int universes);
    ofEvent<string> receivedNode;
    
private:
    bool bStarted;
    ofxArtNet artnet;
    int nUniverses;
    vector<ofxArtNetDmxData> dmxDataPacket;
    vector<unsigned char> linepixels;
    
};


#endif /* pmWarpPiRendererArtNet_h */
