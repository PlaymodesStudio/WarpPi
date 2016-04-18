#include "ofApp.h"
#include "pmWarpPiRendererVideoPlayer.cpp"


//--------------------------------------------------------------
void ofApp::setup() {
    
    ofSetFrameRate(60);
    ofSetVerticalSync(true);

    ofLogToFile("warpPiLog.txt", true);
    ofSetLogLevel(OF_LOG_NOTICE);

    debugPosition = ofVec2f(20,20);
    isDebugging = false;
    isTesting = false;
    useFbo = true;
    showFPS = true;
    
    /// READ CONF
    readConfig();
    
    /// OSC
    oscReceiverOSC.setup(confOscReceivePort);
    oscReceiverSTRING.setup(confOscReceivePortStringMode);
    oscSender.setup(confOscSendAddress,confOscSendPort);
    

    /// RENDERERS
//    int resX = 1280;
//    int resY = 720;
    int resX = ofGetWidth();
    int resY = ofGetHeight();
    
    if(confHasVideo)
    {
        #ifdef TARGET_RASPBERRY_PI
            pmWarpPiRendererOMXPlayer* _video = new pmWarpPiRendererOMXPlayer();
            _video->setup(id,confOscSendAddress,confOscSendPort);
            _video->setupScreen(ofVec2f(0,0), ofVec2f(resX,resY));
            _video->setupOMXPlayer(confVideoFileName, ofVec2f(0,0),ofVec2f(resX,resY));
        #else
            pmWarpPiRendererVideoPlayer* _video = new pmWarpPiRendererVideoPlayer();
            _video->setup(id,confOscSendAddress,confOscSendPort);
            _video->setupScreen(ofVec2f(0,0), ofVec2f(resX,resY));
            _video->setupVideoPlayer(confVideoFileName, ofVec2f(0,0),ofVec2f(resX,resY));
            //_video->setupOMXPlayer(confVideoFileName, ofVec2f(0,0),ofVec2f(1280,720));
        #endif

        renderers.push_back((pmWarpPiRenderer*) _video);
    }
    if(confHasDmx)
    {
        pmWarpPiRendererDmx* _dmx = new pmWarpPiRendererDmx();


        _dmx->setup(id,confOscSendAddress,confOscSendPort);

        _dmx->setupDmx(confDmxDevice,confDmxNumChannels,confDmxFirstChannel);
        
        renderers.push_back((pmWarpPiRenderer*) _dmx);
        
    }
    
    //keyPressed('d');
    //ofSetLogLevel(OF_LOG_SILENT);
}
//--------------------------------------------------------------
void ofApp::readConfig()
{
    ofxXmlSettings configXML;
    configXML.load("./app/config.xml");
    configXML.pushTag("config");
   
    /// ID
    id = configXML.getValue("id","error");
    name = configXML.getValue("name","error");

    /// OSC SETUP
    confOscReceivePort = configXML.getValue("oscReceivePort",-1);
    confOscSendPort = configXML.getValue("oscSendPort",-1);
    confOscSendAddress = configXML.getValue("oscSendAddress","error");
    confOscReceivePortStringMode = configXML.getValue("oscReceivePortStringMode",-1);
    
    
    /// VIDEO FILE
    confVideoFileName = configXML.getValue("videoFileName","error");
    
    /// WHAT IT HAS ?
    confHasVideo = confHasDmx = false;
    if(configXML.getValue("hasVideo","error")=="yes") confHasVideo=true;
    if(configXML.getValue("hasDMX","error")=="yes") confHasDmx=true;
    
    /// DMX
    confDmxDevice = configXML.getValue("dmxDevice",-1);
    confDmxNumChannels = configXML.getValue("dmxNumChannels",-1);
    confDmxFirstChannel = configXML.getValue("dmxFirstChannel",-1);
    
    /// add to LOG
    ofLog(OF_LOG_NOTICE) << "ofApp :: readConfig :: id " << id << " :: name " << name << " :: oscIn " << confOscReceivePort << " :: oscOut IP " << confOscSendAddress << " :: oscOut Port " << confOscSendPort << " :: videoFile " << confVideoFileName << " :: hasVideo " << confHasVideo << " :: hasDMX " << confHasDmx;
    
}
//--------------------------------------------------------------
void ofApp::update()
{
    /// OSC
    
    while( oscReceiverSTRING.hasWaitingMessages() || oscReceiverOSC.hasWaitingMessages() )
    {
        ofxOscMessage* m = new ofxOscMessage();
        
        if(oscReceiverSTRING.hasWaitingMessages())
        {
            ofxOscMessage* mORIG = new ofxOscMessage();
            oscReceiverSTRING.getNextMessage(mORIG);
            
            // print last message on debug window
            lastOscMessage = ofToString(mORIG->getAddress());
            
            for(int i=0;i<mORIG->getNumArgs();i++)
            {
                if(mORIG->getArgType(i)==OFXOSC_TYPE_FLOAT) lastOscMessage = lastOscMessage + " : "  + ofToString(mORIG->getArgAsFloat(i));
                else if (mORIG->getArgType(i)==OFXOSC_TYPE_INT32) lastOscMessage = lastOscMessage + " : "  + ofToString(mORIG->getArgAsInt32(i));
                else if (mORIG->getArgType(i)==OFXOSC_TYPE_STRING) lastOscMessage = lastOscMessage + " : "  + ofToString(mORIG->getArgAsString(i));
            }
            
            ///---------------------------------
            /// PROCESS OSC MESSAGE !! this is converting a full string into separate tokens as it should ... (made for duration easy of edit)
            ///---------------------------------
            m = processOSC(mORIG);
        }
        
        else if (oscReceiverOSC.hasWaitingMessages())
        {
            oscReceiverOSC.getNextMessage(m);
        }
    
        /// UPDATE ALL RENDERERS
        ///---------------------------------
        for(int i=0;i<renderers.size();i++)
        {
            renderers[i]->updateOSC(m);
        }
        
        ///---------------------------------
        /// MESSAGES THAT AFFECT IN GENERAL
        ///---------------------------------
        string address = m->getAddress();
        
        // get the id
        string addressWithoutSlash = address.substr(1,address.size()-1);
        
        if((address=="/all")||(id==addressWithoutSlash))
        {
            /// COMMAND
            string command = m->getArgAsString(0);

            /// DEBUG
            if(command == "debug")
            {
                int val = m->getArgAsInt32(1);
                if(val == 0)
                {
                    /// stop DEBUGGING
                    setDebug(false);
                }
                else if (val ==1)
                {
                    /// start DEBUGGING
                    setDebug(true);
                }
            }
            /// TEST
            else if(command == "test")
            {
                int val = m->getArgAsInt32(1);
                if(val == 0)
                {
                    /// stop TESTING
                    setTest(false);
                }
                else if (val ==1)
                {
                    /// start TESTING
                    setTest(true);
                }
            }
            /// PING
            else if(command == "ping")
            {
                ofxOscMessage m;
                m.setAddress("/pong");
                m.addStringArg(id);
                oscSender.sendMessage(m);
                
                ofLog(OF_LOG_NOTICE) << "pmOmxPlayer :: receveid PING !! answering PONG OMX !! ";
            }
            /// REBOOT
            else if(command == "reboot")
            {
                ofSystem("./data/scripts/reboot.sh");
                cout << " REBOOT ? " << endl;
            }
            /// SHUTDOWN
            else if(command == "shutdown")
            {
                ofSystem("./data/scripts/shutdown.sh");
                cout << " shutdown ? " << endl;
            }
            /// EXIT
            else if(command == "exit")
            {
                //ofSystem("./data/scripts/shutdown.sh");
                cout << " osc received exit !! " << endl;
                std::exit(0);
                this->exit();
            }

        }
    }
}

//-------------------------------------------------------------------------
ofxOscMessage* ofApp::processOSC(ofxOscMessage* m)
{
    ofxOscMessage* myMessage;
    myMessage = new ofxOscMessage();
    
    /// GET DATA FROM MESSAGE
    string address = m->getAddress();
    
    // split first argument ... into tokens <string> vector
    using namespace std;
    string sentence = m->getArgAsString(0);
    istringstream iss(sentence);
    vector<string> tokens;
    copy(istream_iterator<string>(iss),
         istream_iterator<string>(),
         back_inserter(tokens));
    
    /// FILL MY MESSAGE TO BE PROCESSED
    myMessage->setAddress(m->getAddress());
    int numTokens = tokens.size();
    
    
    bool isADurationMessage = m->getAddress().find("/duration/info")!=-1;
    if(!isADurationMessage)
    {
        cout << "omxPlayer :: update OSC ::"  << address << " :"  << sentence << endl;
        
        if(numTokens==0)
        {
            cout << "omxPlayer :: update OSC :: OSC MESSAGE WITH NO ARGUMENTS AT ALL ? " << address << endl;
        }

        ///---------------------------------
        /// 1 TOKENS : 1 COMMAND NO PARAMS
        ///---------------------------------

        else if(numTokens==1)
        {
            // 1 token means just a command like : pause, saveQuad, resetQuad, nextQuadPoint, preQuadPoint, ping, shutdown, reboot, debug
            // so just pass though the command string
            cout << " 1 Argument  " << address << " : " << m->getArgAsString(0) << endl;
            
            /// (0) is the command
            /// ----------------------------
            myMessage->addStringArg(tokens[0]);
        }

        ///---------------------------------
        /// 2 TOKENS : 1 COMMAND 1 PARAMETER
        ///---------------------------------

        else if(numTokens==2)
        {
            cout << " 2 Argument  " << address << " : " << m->getArgAsString(0) << endl;
            
            /// (0) is the command string
            /// ----------------------------

            myMessage->addStringArg(tokens[0]);

            if((tokens[0]=="test")||(tokens[0]=="editQuad")||(tokens[0]=="debug")
               ||(tokens[0]=="movePointUp")||(tokens[0]=="movePointDown")||(tokens[0]=="movePointRight")||(tokens[0]=="movePointLeft"))
            {
                /// 2 ARGUMENTS 0/ command string 1/int
                /// ----------------------------
                // (1) is an int on this commands
                int val = ofToInt(tokens[1]);
                myMessage->addIntArg(val);
            }
            else if ((tokens[0]=="play")||(tokens[0]=="stop")||(tokens[0]=="restart"))
            {
                /// 2 ARGUMENTS 0/ command string 1/float
                /// ----------------------------
                // (1) is an float on this commands
                float valF = ofToFloat(tokens[1]);
                myMessage->addFloatArg(valF);
                
            }
            
            
        }
        
        ///---------------------------------
        /// 3 TOKENS : 1 COMMAND 2 PARAMETER
        ///---------------------------------

        else if(numTokens==3)
        {
            cout << " 3 Argument  " << address << " : " << m->getArgAsString(0) << endl;
            
            /// (0) is the command string
            /// ----------------------------
            myMessage->addStringArg(tokens[0]);
            
            if((tokens[0]=="setAllDMXCh"))
            {
                /// 3 ARGUMENTS 0/ command string 1/int 2/float
                /// ----------------------------

                // (1) is an int on this commands
                int val = ofToInt(tokens[1]);
                myMessage->addIntArg(val);
                
                // (2) is an float on this commands
                float valF = ofToFloat(tokens[2]);
                myMessage->addFloatArg(valF);
            }
            else if((tokens[0]=="load"))
            {
                /// 3 ARGUMENTS 0/ command string /1 string 2/float
                /// ----------------------------
                                
                // (1) is an int on this commands
                string val = ofToString(tokens[1]);
                myMessage->addStringArg(val);
                
                // (2) is an float on this commands
                float valF = ofToFloat(tokens[2]);
                myMessage->addFloatArg(valF);
            }


            
        }
        
        ///---------------------------------
        /// 4 TOKENS : 1 COMMAND 3 PARAMETER
        ///---------------------------------
        
        else if(numTokens==4)
        {
            cout << " 4 Argument  " << address << " : " << m->getArgAsString(0) << endl;
            
            /// (0) is the command string
            /// ----------------------------
            myMessage->addStringArg(tokens[0]);
         
            if((tokens[0]=="setDMXCh"))
            {
                /// 4 ARGUMENTS 0/ command string 1/int /2 int 3/float
                /// ----------------------------
                
                // (1) is an int on this commands
                int val = ofToInt(tokens[1]);
                myMessage->addIntArg(val);

                // (2) is an int on this commands
                int val2 = ofToInt(tokens[2]);
                myMessage->addIntArg(val2);

                // (3) is an float on this commands
                float valF = ofToFloat(tokens[3]);
                myMessage->addFloatArg(valF);
            }
            
            
        }

        cout << " ...................." << endl;
        
        return(myMessage);
    }
    return(myMessage);
}
//--------------------------------- -----------------------------
void ofApp::draw(){

    if(renderers[0]->isDebugging) ofBackground(32);
    else ofBackground(0);

	if(!useFbo)
    {
        ofSetColor(255);
        #ifdef TARGET_RASPBERRY_PI
        {
            pmWarpPiRendererOMXPlayer* o = (pmWarpPiRendererOMXPlayer*) renderers[0];
            o->omxPlayer->draw(0, 0, ofGetWidth(), ofGetHeight());
        }
        #endif
        
        #ifdef TARGET_OS_MAC
        {
            pmWarpPiRendererVideoPlayer* v = (pmWarpPiRendererVideoPlayer*) renderers[0];
            v->videoPlayer->draw(0,0,ofGetWidth(),ofGetHeight());
        }
        #endif
    }
    else
    {
        for(int i=0;i<renderers.size();i++)
        {
            renderers[i]->draw();

        }
        
        if(isDebugging)
        {
            showDebug();
        }
    }
    if(showFPS)
    {
        ofSetColor(255);
        ofDrawBitmapString(ofToString(ofGetFrameRate()),ofGetWidth()-50,ofGetHeight()-50);
    }
		
}

//--------------------------------------------------------------
void ofApp::exit() {
	
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{

    if (key=='d')
    {
        toggleDebug();
    }
    else if (key=='t')
    {
        toggleTest();
    }
    else if (key=='p')
    {
        #ifdef TARGET_RASPBERRY_PI
            pmWarpPiRendererOMXPlayer* vp = (pmWarpPiRendererOMXPlayer*) renderers[0];
            vp->omxPlayer->setPaused(false);
        #else
            pmWarpPiRendererVideoPlayer* vp = (pmWarpPiRendererVideoPlayer*) renderers[0];
            vp->videoPlayer->setPaused(false);
        #endif
        

    }
    else if (key=='h')
    {
	    pmWarpPiRendererScreen* s = (pmWarpPiRendererScreen*) renderers[0];
	    s->doHomography=!s->doHomography;
    }    
    else if (key=='e')
    {
        pmWarpPiRendererScreen* s = (pmWarpPiRendererScreen*) renderers[0];
        s->doEditQuadPoints=!s->doEditQuadPoints;
    }
    else if (key=='f')
    {
        showFPS=!showFPS;
    }
    else if (key=='o')
    {
        useFbo = !useFbo;
    }
   
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void ofApp::showDebug()
{
    int lineHeight = 15;
    int whichHeight = debugPosition.y;
    
    
    ofSetColor(255,128,0);
    ofDrawBitmapString("RENDERER",debugPosition.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofSetColor(255);
    ofDrawBitmapString("FPS : " +ofToString(int(ofGetFrameRate())),debugPosition.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofDrawBitmapString("ID : " +ofToString(id),debugPosition.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofDrawBitmapString("NAME : " + name,debugPosition.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofDrawBitmapString("OSC RECEIVE PORT: "  + ofToString(confOscReceivePort),debugPosition.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofDrawBitmapString("OSC SEND TO ADDRESS : "  + ofToString(confOscSendAddress),debugPosition.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofDrawBitmapString("OSC SEND TO PORT : "  + ofToString(confOscSendPort),debugPosition.x,whichHeight);
    
    whichHeight=whichHeight + lineHeight;
    ofDrawBitmapString("LAST OSC MSG : "  + lastOscMessage,debugPosition.x,whichHeight);

    
}

//--------------------------------------------------------------
void ofApp::setDebug(bool b)
{
    this->isDebugging = b;
    for(int i=0;i<renderers.size();i++)
    {
        //pmWarpPiRendererVideoPlayer* vp = (pmWarpPiRendererVideoPlayer*) renderers[i];
        //vp->isDebugging = isDebugging;
        //renderers[i]->isDebugging = this->isDebugging;
        renderers[i]->setIsDebugging(b);
    }
    
}

//--------------------------------------------------------------
void ofApp::toggleDebug()
{
    this->isDebugging = ! this->isDebugging;
    for(int i=0;i<renderers.size();i++)
    {
        //pmWarpPiRendererVideoPlayer* vp = (pmWarpPiRendererVideoPlayer*) renderers[i];
        //vp->isDebugging = isDebugging;
        //renderers[i]->isDebugging = this->isDebugging;
        renderers[i]->setIsDebugging(this->isDebugging);

    }

    
}
//--------------------------------------------------------------
void ofApp::setTest(bool b)
{
    this->isTesting = b;
    for(int i=0;i<renderers.size();i++)
    {
        //pmWarpPiRendererVideoPlayer* vp = (pmWarpPiRendererVideoPlayer*) renderers[i];
        //vp->isDebugging = isDebugging;
        //renderers[i]->isTesting = this->isTesting;
        renderers[i]->setIsTesting(b);
        
    }
    
}
//--------------------------------------------------------------
void ofApp::toggleTest()
{
    this->isTesting = ! this->isTesting;
    for(int i=0;i<renderers.size();i++)
    {
        //pmWarpPiRendererVideoPlayer* vp = (pmWarpPiRendererVideoPlayer*) renderers[i];
        //vp->isDebugging = isDebugging;
        //renderers[i]->isTesting = this->isTesting;
        renderers[i]->setIsTesting(this->isTesting);

    }

}
