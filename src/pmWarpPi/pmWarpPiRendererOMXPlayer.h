#pragma once
#ifndef PM_WARPI_RENDERER_OMXPLAYER
#define PM_WARPI_RENDERER_OMXPLAYER

#ifdef TARGET_RASPBERRY_PI

#include "ofxOMXPlayer.h"
#include "pmWarpPiRendererVideoPlayer.h"

class pmWarpPiRendererOMXPlayer : public pmWarpPiRendererVideoPlayer
{
    
public:
 
    pmWarpPiRendererOMXPlayer(){pmWarpPiRendererVideoPlayer();};
    
    //override methods
    virtual void createPlayer(){
        omxPlayer = new ofxOMXPlayer();
        ofLog(OF_LOG_NOTICE) << "pmOmxPlayer :: New OmxPlayer";
        
    } 
    
    virtual void loadAndPlay(){
        string videoPath = ofToDataPath(videoFileName, true);
        //Somewhat like ofFboSettings we may have a lot of options so this is the current model
        ofxOMXPlayerSettings settings;
        settings.videoPath = videoPath;
        settings.useHDMIForAudio	= false;		//default true
        settings.enableTexture		= true;		//default true
        settings.enableLooping		= true;		//default true
        settings.enableAudio		= true;		//default true, save resources by disabling
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
    } 
    
    virtual void setPlayerVolume(float volume){
        omxPlayer->setVolume(volume);
    } 
    
    virtual void updatePlayer(){
        //It auto updates
    }
    
    virtual void drawPlayer(int x, int y, int width, int height){
        omxPlayer->draw(x, y, width, height);
    }
    
    virtual void drawPlayer(){
        drawPlayer(videoPosition.x,videoPosition.y,videoSize.x,videoSize.y);
    }
    
    virtual void playPlayer(){
        setPlayerPaused(false);
    }
    
    virtual bool isPlayerPaused(){
        return omxPlayer->isPaused();
    }
    
    virtual void stopPlayer(){
        setPlayerPaused(true);
    }
    
    virtual void closePlayer(){
        omxPlayer->close();
    }
    
    virtual void setPlayerPaused(bool paused){
        omxPlayer->setPaused(paused);
    }
    
    virtual void setPlayerPosition(float pct){
//        omxPlayer->setPosition(pct);
    }
    
    virtual int getPlayerHeight(){
        return omxPlayer->getHeight();
    }
    
    virtual int getPlayerWidth(){
        return omxPlayer->getWidth();
    }
    
    virtual int getPlayerCurrentFrame(){
        return omxPlayer->getCurrentFrame();
    }
    
    virtual int getPlayerTotalNumFrames(){
        return omxPlayer->getTotalNumFrames();
    }
    
    virtual ofLoopType getPlayerLoopState(){
//        if(omxPlayer->isLoopingEnabled()) return OF_LOOP_NORMAL;
//        else return OF_LOOP_NONE;
        return OF_LOOP_NORMAL;
    }
    
    
    
private:
    
    ofxOMXPlayer*       omxPlayer;
    
    
    
};
#endif
#endif
