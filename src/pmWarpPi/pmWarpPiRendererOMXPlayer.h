#pragma once
#ifndef PM_WARPI_RENDERER_OMXPLAYER
#define PM_WARPI_RENDERER_OMXPLAYER

#ifdef TARGET_RASPBERRY_PI

#include "ofxOMXPlayer.h"
#include "pmWarpPiRendererVideoPlayer.h"
#include "ofMain.h"

class pmWarpPiRendererOMXPlayer : public pmWarpPiRendererVideoPlayer
{
    
public:
 
    pmWarpPiRendererOMXPlayer(){pmWarpPiRendererVideoPlayer();};
    
    //override methods
    virtual void createPlayer(){
        videoPlayer = new ofxOMXPlayer();
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
        videoPlayer->setup(settings);
    } override
    
    virtual void playPlayer(){
        setPlayerPaused(false);
    }override
    
    virtual void updatePlayer(){
        //It auto updates
    }override
    
    virtual void stopPlayer(){
        setPlayerPaused(true);
    }override
    
    virtual ofLoopType getPlayerLoopState(){
        if(videoPlayer->isLoopingEnabled) return OF_LOOP_NORMAL;
        else return OF_LOOP_NONE;
    }override
    
    
    
private:
    
    
    ofxOMXPlayer*       videoPlayer;
    
    
    
};
#endif
#endif
