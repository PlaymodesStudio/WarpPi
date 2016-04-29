#include "ofMain.h"
#include "ofApp.h"


//========================================================================
int main( ){
#ifdef TARGET_RASPBERRY_PI
    ofSetupOpenGL(1920, 1080, OF_FULLSCREEN);
#else
	ofSetupOpenGL(1280,720, OF_WINDOW);			// <-------- setup the GL context
#endif
    
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new ofApp());

    
  /*
    ofGLESWindowSettings settings;
    settings.width = 1280;
    settings.height = 720;
    settings.setGLESVersion(2);
    ofCreateWindow(settings);
    
    ofRunApp( new testApp());
   */
    
}
