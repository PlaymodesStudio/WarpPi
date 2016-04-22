#pragma once
#ifndef PM_WARPI_RENDERER_VIDEOPLAYER
#define PM_WARPI_RENDERER_VIDEOPLAYER

#include "pmWarpPiRendererScreen.h"
#include "ofMain.h"

class pmWarpPiRendererVideoPlayer : public pmWarpPiRendererScreen
{
    
public:
 
    pmWarpPiRendererVideoPlayer();
    virtual void        updateOSC(ofxOscMessage* m);
    virtual void        deleteRenderer();

    /// CLASS FUNCTIONS
    ////////////////////
    virtual void        updateForScreen();

    virtual void        drawIntoFbo();
    virtual void        setupVideoPlayer(string _name,ofVec2f _pos, ofVec2f _size);
    virtual void        onComplete(float* arg);
    virtual void        showDebug();
//    void                showVideoPlayerDebug();
    
    //methods that have to be overwrite by omxplayer class
    virtual void createPlayer(){
        videoPlayer = new ofVideoPlayer();
        cout<<"New Video Player"<<endl;
    }
    virtual void loadAndPlay(){
        videoPlayer->load(videoFileName);
        playPlayer();
    };
    virtual void setPlayerVolume(float volume){
        videoPlayer->setVolume(volume);
    };
    virtual void updatePlayer(){
        videoPlayer->update();
    };
    virtual void drawPlayer(){
        videoPlayer->draw(videoPosition.x,videoPosition.y,videoSize.x,videoSize.y);
    };
    virtual void playPlayer(){
        videoPlayer->play();
    };
    virtual bool isPlayerPaused(){
        return videoPlayer->isPaused();
    };
    virtual void setPlayerPaused(bool paused){
        videoPlayer->setPaused(paused);
    }
    
    void keyPressed(ofKeyEventArgs &a);
    void keyReleased(ofKeyEventArgs &a);
//    void mouseMoved(ofMouseEventArgs &a);
//    void mouseDragged(ofMouseEventArgs &a);
//    void mousePressed(ofMouseEventArgs &a);
//    void mouseReleased(ofMouseEventArgs &a);
    void mouseMoved(ofMouseEventArgs & args);
    void mouseDragged(ofMouseEventArgs & args);
    void mousePressed(ofMouseEventArgs & args);
    void mouseReleased(ofMouseEventArgs & args);
    void mouseScrolled(ofMouseEventArgs & args){};
    void mouseEntered(ofMouseEventArgs & args){};
    void mouseExited(ofMouseEventArgs & args){};

    /// CLASS PARAMS
    /////////////////
    ofVideoPlayer*      videoPlayer;
    string              videoFileName;
    ofVec2f             videoPosition;
    ofVec2f             videoSize;
    ofVec2f             videoPlayerDebugPosition;
    
    
    
};
#endif