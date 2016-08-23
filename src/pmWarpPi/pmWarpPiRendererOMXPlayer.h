#pragma once
#ifndef PM_WARPI_RENDERER_OMXPLAYER
#define PM_WARPI_RENDERER_OMXPLAYER

#ifdef TARGET_RASPBERRY_PI

#include "ofxOMXPlayer.h"
#include "pmWarpPiRendererVideoPlayer.h"

class pmWarpPiRendererOMXPlayer : public pmWarpPiRendererVideoPlayer
{
    
public:
 
    pmWarpPiRendererOMXPlayer(){
        videoFileName = "";
        videoPosition = ofVec2f(0,0);
        videoSize = ofVec2f(0,0);
        videoPlayerDebugPosition = ofVec2f(520,20);
        textureMode = true;
        hasAudio = true;
    };
    
    //override methods
    void createPlayer(){
        omxPlayer = new ofxOMXPlayer();
        ofLog(OF_LOG_NOTICE) << "pmOmxPlayer :: New OmxPlayer";
        
        string videoPath = ofToDataPath(videoFileName, true);
        //Somewhat like ofFboSettings we may have a lot of options so this is the current model
        ofxOMXPlayerSettings settings;
        settings.videoPath = videoPath;
        settings.useHDMIForAudio	= true;		//default true
        settings.enableTexture		= textureMode;		//default true
        settings.enableLooping		= loop;		//default true
        settings.enableAudio		= hasAudio;		//default true, save resources by disabling
        //settings.doFlip     Texture = true;		//default false
        
        if (!settings.enableTexture)
        {
            /*
             We have the option to pass in a rectangle
             to be used for a non-textured player to use (as opposed to the default full screen)
             */
            settings.displayRect.width = videoSize.x;
            settings.displayRect.height = videoSize.y;
            settings.displayRect.x = 0;
            settings.displayRect.y = 0;
        }
        //so either pass in the settings
        omxPlayer->setup(settings);
    };
    
    void loadMovie(){
        string videoPath = ofToDataPath(videoFileName, true);
        ofxOMXPlayerSettings settings;
        settings.videoPath = videoPath;
        settings.useHDMIForAudio	= true;		//default true
        settings.enableTexture		= textureMode;		//default true
        settings.enableLooping		= loop;		//default true
        settings.enableAudio		= hasAudio;		//default true, save resources by disabling
        //settings.doFlip     Texture = true;		//default false
        
        if (!settings.enableTexture)
        {
            /*
             We have the option to pass in a rectangle
             to be used for a non-textured player to use (as opposed to the default full screen)
             */
            settings.displayRect.width = videoSize.x;
            settings.displayRect.height = videoSize.y;
            settings.displayRect.x = 0;
            settings.displayRect.y = 0;
        }
        //so either pass in the settings
        omxPlayer->setup(settings);
//        omxPlayer->loadMovie(videoPath);
    };
    
    void setPlayerVolume(float volume){
        audioVolume = volume;
        omxPlayer->setVolume(volume);
    };
    
    void updatePlayer(){
        //It auto updates
    };
    
    virtual void drawPlayer(ofRectangle container){
        omxPlayer->draw(container.x, container.y, container.width, container.height);
    };
    
    void drawPlayer(int x, int y, int width, int height){
        omxPlayer->draw(x, y, width, height);
    };
    
    void drawPlayer(){
        drawPlayer(videoPosition.x,videoPosition.y,videoSize.x,videoSize.y);
    };
    
    void playPlayer(){
        setPlayerPaused(false);
    };
    
    bool isPlayerPaused(){
        return omxPlayer->isPaused();
    };
    
    void stopPlayer(){
        setPlayerPaused(true);
    };
    
    void restartMovie(){
        omxPlayer->restartMovie();
    }
    
    void closePlayer(){
        omxPlayer->close();
    };
    
    void setPlayerPaused(bool paused){
        omxPlayer->setPaused(paused);
    };
    
    void setPlayerPosition(float pct){
//        omxPlayer->setPosition(pct);
    };
    
    int getPlayerHeight(){
        return omxPlayer->getHeight();
    };
    
    int getPlayerWidth(){
        return omxPlayer->getWidth();
    };
    
    int getPlayerCurrentFrame(){
        return omxPlayer->getCurrentFrame();
    };
    
    int getPlayerTotalNumFrames(){
        return omxPlayer->getTotalNumFrames();
    };
    
    ofLoopType getPlayerLoopState(){
        if(loop) return OF_LOOP_NORMAL;
        else return OF_LOOP_NONE;
        return OF_LOOP_NORMAL;
    };
    
    virtual void setPlayerLoop(bool loop){
        
    };
    
    void setTextured(bool textured){
        textureMode = textured;
    }
    
    void setAudio(bool audio){
        hasAudio = audio;
    }
    
    void setPlayerLoop(bool _loop){
        loop = _loop;
    }
    
    
    
private:
    
    ofxOMXPlayer*       omxPlayer;
    bool    textureMode, hasAudio, loop;
    
    
    
};
#endif
#endif
