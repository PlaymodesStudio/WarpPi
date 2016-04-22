#pragma once
#ifndef PM_WARPI_RENDERER_OMXPLAYER
#define PM_WARPI_RENDERER_OMXPLAYER

#ifdef TARGET_RASPBERRY_PI

#include "ofxOMXPlayer.h"
#include "pmWarpPiRendereromxPlayer.h"
#include "ofMain.h"

class pmWarpPiRendererOMXPlayer : public pmWarpPiRendereromxPlayer
{
    
public:
 
    pmWarpPiRendererOMXPlayer(){pmWarpPiRendereromxPlayer();};
    
    //override methods
    virtual void createPlayer(){
        omxPlayer = new ofxOMXPlayer();
        ofLog(OF_LOG_NOTICE) << "pmOmxPlayer :: New OmxPlayer";
        
    } override
    
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
    } override
    
    virtual void setPlayerVolume(float volume){
        omxPlayer->setVolume(volume);
    } override
    
    virtual void updatePlayer(){
        //It auto updates
    }override
    
    virtual void drawPlayer(int x, int y, int width, int height){
        omxPlayer->draw(x, y, width, height);
    }override
    
    virtual void playPlayer(){
        setPlayerPaused(false);
    }override
    
    virtual bool isPlayerPaused(){
        return omxPlayer->isPaused();
    }override
    
    virtual void stopPlayer(){
        setPlayerPaused(true);
    }override
    
    virtual void closePlayer(){
        omxPlayer->close();
    }override
    
    virtual void setPlayerPaused(bool paused){
        omxPlayer->setPaused(paused);
    }override
    
    virtual void setPlayerPosition(float pct){
        omxPlayer->setPosition(0.0);
    }override
    
    virtual int getPlayerHeight(){
        return omxPlayer->getHeight();
    }override
    
    virtual int getPlayerWidth(){
        return omxPlayer->getWidth();
    }override
    
    virtual int getPlayerCurrentFrame(){
        return omxPlayer->getCurrentFrame();
    }override
    
    virtual int getPlayerTotalNumFrames(){
        return omxPlayer->getTotalNumFrames();
    }override
    
    virtual ofLoopType getPlayerLoopState(){
        if(omxPlayer->isLoopingEnabled) return OF_LOOP_NORMAL;
        else return OF_LOOP_NONE;
    }override
    
    
    
private:
    
    ofxOMXPlayer*       omxPlayer;
    
    
    
};
#endif
#endif
