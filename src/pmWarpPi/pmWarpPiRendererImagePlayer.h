#pragma once
#ifndef PM_WARPI_RENDERER_IMAGEPLAYER
#define PM_WARPI_RENDERER_IMAGEPLAYER

#include "pmWarpPiRendererDrawable.h"
#include "ofMain.h"



class pmWarpPiRendererImagePlayer : public pmWarpPiRendererDrawable
{
    
public:
 
    pmWarpPiRendererImagePlayer();
    virtual void        updateOSC(ofxOscMessage* m);
    virtual void        deleteRenderer();

    /// CLASS FUNCTIONS
    ////////////////////
    virtual void        updateForScreen();

    virtual void        drawElement(ofRectangle container);
    virtual void        setupImagePlayer(string _name, bool active = false);
    virtual void        onComplete(float* arg);
    virtual void        onCrossFadeComplete(float* arg);
    virtual void        showDebug();
    
    void stopImagePlayer(float fadeTime);
    void startImagePlayer(float fadeTime);
    
    bool loadImages();
    void drawImage(int index, float alpha, ofRectangle container);

    /// CLASS PARAMS
    /////////////////
    ofDirectory     dir;
    deque<ofImage>  images;
    vector<string>  imagesInFolderPaths;
    string          imagePath;
    bool            folderPlay; //if it's true plays a folder, else plays only one image;
    string          folderName;
    
    int             currentImage, nextImage;
    bool            hasMedia;
    float           crossFadeAlpha;
    
    float           beginImageTime; // store when we start time timer
    float           nextImageTime; // when do want to stop the timer
    
    float           fadeTime;
    
};
#endif