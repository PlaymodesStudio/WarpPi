#pragma once
#ifndef PM_WARPI_RENDERER_VIDEOPLAYER
#define PM_WARPI_RENDERER_VIDEOPLAYER

#include "pmWarpPiRendererDrawable.h"
#include "ofMain.h"

class pmWarpPiRendererVideoPlayer : public pmWarpPiRendererDrawable
{
    
public:
 
    pmWarpPiRendererVideoPlayer();
    virtual void        updateOSC(ofxOscMessage* m);
    virtual void        deleteRenderer();

    /// CLASS FUNCTIONS
    ////////////////////
    virtual void        updateForScreen();

    virtual void        drawElement(ofRectangle container);
    virtual void        setupVideoPlayer(string _name, bool active = true);
    virtual void        onComplete(float* arg);
    virtual void        onFadeOutComplete(float* arg);
    virtual void        showDebug();
//    void                showVideoPlayerDebug();
    
    
    void stopVideoPlayer(float fadeTime);
    void startVideoPlayer(float fadeTime);
    
    //methods that have to be overwrite by omxplayer class
    virtual void createPlayer(){
        videoPlayer = new ofVideoPlayer();
        cout<<"New Video Player"<<endl;
    };
    virtual void loadMovie(){
        videoPlayer->load(videoFileName);
        setPlayerLoop(loop);
        playPlayer();
    };
    virtual void setPlayerVolume(float volume){
        //audioVolume = volume;
        videoPlayer->setVolume(volume);
    };
    virtual void updatePlayer(){
        videoPlayer->update();
    };
    virtual void drawPlayer(ofRectangle container){
        videoPlayer->draw(container);
    };
    virtual void drawPlayer(int x, int y, int width, int height){
        videoPlayer->draw(x, y, width, height);
    };
    virtual void drawPlayer(){
        drawPlayer(videoPosition.x,videoPosition.y,videoSize.x,videoSize.y);
    };
    virtual void playPlayer(){
        videoPlayer->play();
    };
    virtual bool isPlayerPaused(){
        return videoPlayer->isPaused();
    };
    virtual void stopPlayer(){
        videoPlayer->stop();
    };
    
    virtual void restartMovie(){
        videoPlayer->setPosition(0);
    }
    virtual void closePlayer(){
        videoPlayer->close();
    };
    virtual void setPlayerPaused(bool paused){
        videoPlayer->setPaused(paused);
    };
    virtual void setPlayerPosition(float pct){
        videoPlayer->setPosition(pct);
    };
    virtual int getPlayerHeight(){
        return videoPlayer->getHeight();
    };
    virtual int getPlayerWidth(){
        return videoPlayer->getWidth();
    };
    virtual int getPlayerCurrentFrame(){
        return videoPlayer->getCurrentFrame();
    };
    virtual int getPlayerTotalNumFrames(){
        return videoPlayer->getTotalNumFrames();
    };
    virtual ofLoopType getPlayerLoopState(){
        return videoPlayer->getLoopState();
    };
    virtual void setPlayerLoop(bool loop){
        this->loop = loop;
        if(loop)
            videoPlayer->setLoopState(OF_LOOP_NORMAL);
        else
            videoPlayer->setLoopState(OF_LOOP_NONE);
    };
    
    ofPixels& getVideoPixels(){
        return videoPlayer->getPixels();
    };
    

private:
    /// CLASS PARAMS
    /////////////////
    ofVideoPlayer*      videoPlayer;
    
protected:
    string              videoFileName;
    ofVec2f             videoPosition;
    ofVec2f             videoSize;
    ofVec2f             videoPlayerDebugPosition;
    float               fadeTime;
    bool                loop;
    
    
};
#endif
