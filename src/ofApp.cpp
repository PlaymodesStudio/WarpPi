#include "ofApp.h"
#include "pmLocalAddressGrabber.h"
#include <typeinfo>

//--------------------------------------------------------------
void ofApp::setup()
{
    ofHideCursor();
    ofSetFrameRate(60);
    ofSetVerticalSync(true);

    // LOG FILE
    ofLogToFile("warpPiLog.txt", true);
    ofSetLogLevel(OF_LOG_NOTICE);

    debugPosition = ofVec2f(20,20);
    isDebugging = false;
    isTesting = false;
    useFbo = true;
    showFPS = true;
    //ofHideCursor();
    
    /// READ CONF
    readConfig();
    
    /// OSC
    oscReceiverOSC.setup(confOscReceivePort);
    oscReceiverSTRING.setup(confOscReceivePortStringMode);
    //oscSender.setup(confOscSendIpAddress,confOscSendPort);
    
    /// TCP
    if(confUsesTCP)
    {
        tcpAreWeConnected = tcpClient.setup(confTCPSendIpAddress, confTCPPort);
        tcpConnectTime=15;
        tcpDeltaTime=0;
        tcpMsgRx = "";
        
        // we send an awake message once we're connected (so Master can ping and update list of connected slaves)
        if(tcpAreWeConnected) sendTCPAwake();
    }

    screen.setup(confHasVideo, confHasImage, useFbo, doHomography, confTextureMode, confHasAudio, confVideoFileName);
        
    // HAS DMX ?
    ///////////////
    if(confHasDmx)
    {
        pmWarpPiRendererDmx* _dmx = new pmWarpPiRendererDmx();
        _dmx->setup(id);
        _dmx->setupDmx(confDmxDevice,confDmxNumChannels,confDmxFirstChannel);
        renderers.push_back((pmWarpPiRenderer*) _dmx);
    }
    
    if(confHasArtNet)
    {
        pmWarpPiRendererArtNet* _artnet = new pmWarpPiRendererArtNet();
        _artnet->setup(id);
        _artnet->setupArtNet(confArtNetFileName, pmLocalAddressGrabber::getMyProbableIpAddress(), confArtNetDestIp, confArtNetSubNet, confArtNetUniverse);
        renderers.push_back((pmWarpPiRenderer*) _artnet);
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
    confOscSendIpAddress = configXML.getValue("oscSendAddress","error");
    confOscReceivePortStringMode = configXML.getValue("oscReceivePortStringMode",-1);
    
    /// VIDEO SETTINGS
    confVideoFileName = configXML.getValue("videoFileName","error");
    useFbo = configXML.getValue("useFbo", "false") == "yes" ? true: false;
    doHomography = configXML.getValue("doHomography", "no") == "yes" ? true: false;
    confTextureMode  = configXML.getValue("textureMode", "yes") == "yes" ? true : false;
    confHasAudio = configXML.getValue("hasAudio", "yes") == "yes" ? true : false;
    
    /// WHAT IT HAS ?
    confHasVideo    = confHasDmx = false;
    
    confHasVideo    = configXML.getValue("hasVideo" , "error")  == "yes" ? true : false;
    
    confHasImage    = configXML.getValue("hasImage" , "error")  == "yes" ? true : false;

    confHasDmx      = configXML.getValue("hasDMX" , "error")    == "yes" ? true : false;
    
    confHasArtNet   = configXML.getValue("hasArtNet" , "false") == "yes" ? true : false;
    
    /// DMX
    confDmxDevice = configXML.getValue("dmxDevice",-1);
    confDmxNumChannels = configXML.getValue("dmxNumChannels",-1);
    confDmxFirstChannel = configXML.getValue("dmxFirstChannel",-1);
    
    /// ARTNET
    confArtNetDestIp = configXML.getValue("artnetDestIp", "127.0.0.1");
    confArtNetFileName = configXML.getValue("artnetFileName", "videos/artnetTest.mov");
    confArtNetUniverse = configXML.getValue("artnetUniverse", 0);
    confArtNetSubNet = configXML.getValue("artnetSubNet", 0);
    
    /// TCP
    if(configXML.getValue("useTCP","error")=="yes") confUsesTCP=true;
    else confUsesTCP = false;
    confTCPSendIpAddress = configXML.getValue("TCPIp","default");
    confTCPPort = configXML.getValue("TCPPort",11111);
    
    /// add to LOG
    ofLog(OF_LOG_NOTICE) << "ofApp :: readConfig :: id " << id << " :: name " << name << " :: oscIn " << confOscReceivePort << " :: oscOut IP " << confOscSendIpAddress << " :: oscOut Port " << confOscSendPort << " :: videoFile " << confVideoFileName << " :: hasVideo " << confHasVideo << " :: hasDMX " << confHasDmx;
    
}
//--------------------------------------------------------------
void ofApp::update()
{
    screen.update();
    /// OSC
    
    while( oscReceiverSTRING.hasWaitingMessages() || oscReceiverOSC.hasWaitingMessages() )
    {
        ofxOscMessage* m = new ofxOscMessage();
        
        // OSC RECEIVER STRING (FOR EASY EDITING FROM DURATION)
        ////////////////////////
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

        // OSC RECEIVER
        /////////////////
        else if (oscReceiverOSC.hasWaitingMessages())
        {
            oscReceiverOSC.getNextMessage(m);
        }
    
        /// UPDATE ALL RENDERERS
        ///////////////////////////
        screen.updateOSC(m);
        for(int i=0;i<renderers.size();i++)
        {
            renderers[i]->updateOSC(m);
        }
        
        ///////////////////////////
        /// MESSAGES THAT AFFECT IN GENERAL
        ///////////////////////////
        string address = m->getAddress();
        
        // get the id
        string addressWithoutSlash = address.substr(1,address.size()-1);
        
        if((address=="/all")||(id==addressWithoutSlash))
        {
            /// COMMAND
            string command = m->getArgAsString(0);

            /// PING
            if(command == "ping")
            {
                ofxOscMessage m;
                m.setAddress("/pong");
                m.addStringArg(id);
                m.addStringArg(name);
                m.addStringArg("192.168.1.44");
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
                this->exit();
                std::exit(0);
            }
            /// CLOSE connection
            else if(command == "close")
            {
                cout << " received close connection" << endl;
                tcpClient.close();
            }

        }
    }
    
    //////////
    /// TCP
    //////////
    
    if(confUsesTCP)
    {
        if(tcpAreWeConnected)
        {
            string str = tcpClient.receive();
            if( str.length() > 0 ){
                tcpMsgRx = str;
                
                ofLog(OF_LOG_NOTICE) << "testApp :: Update : tcpMsg = " << tcpMsgRx << " : " << ofGetElapsedTimef() << endl;
                
                ofxOscMessage* mTcp = new ofxOscMessage();
                
                mTcp = processTCP(tcpMsgRx);
                
                /// UPDATE ALL RENDERERS
                ///---------------------------------
                screen.updateOSC(mTcp);
                for(int i=0;i<renderers.size();i++)
                {
                    renderers[i]->updateOSC(mTcp);
                }
                
                
                
            }
            
            if(!tcpClient.isConnected())
            {
                tcpAreWeConnected = false;
            }
        }
        else
        {
            //if we are not connected lets try and reconnect every 15 seconds
            tcpDeltaTime = ofGetElapsedTimeMillis() - tcpConnectTime;
            
            if( tcpDeltaTime > 5000 )
            {
                cout << "We're not connected to TCP ..." << endl;
                tcpAreWeConnected = tcpClient.setup(confTCPSendIpAddress, confTCPPort);
                tcpConnectTime = ofGetElapsedTimeMillis();
                if(!tcpAreWeConnected)
                {
                    tcpClient.close();
                }
                else
                {
                    // we send an awake message once we're connected (so Master can ping and update list of connected slaves)
                    sendTCPAwake();
                }
                ofLog(OF_LOG_NOTICE) << "Trying to reconnect TCP...." << endl;
                cout << "Trying to reconnect TCP at : "  << confTCPSendIpAddress << " : " << confTCPPort << endl;
            }
            
        }
    }
}

//-------------------------------------------------------------------------
ofxOscMessage* ofApp::processTCP(string tcpString)
{
    ofxOscMessage* myMessage;
    myMessage = new ofxOscMessage();
    
    // split the string received ... into tokens <string> vector
    using namespace std;
    string sentence = tcpString;
    istringstream iss(sentence);
    vector<string> tokens;
    copy(istream_iterator<string>(iss),
         istream_iterator<string>(),
         back_inserter(tokens));
    
    /// FILL MY MESSAGE ADRESS TO BE PROCESSED
    myMessage->setAddress("/" +tokens[0]);
    int numTokens = tokens.size();
    
    // JUST TAKE CARE OF THE TCP MESSAGE IF IT's FOR THIS PI id
    if( ( (id==tokens[0]) || ("all"==tokens[0]) ) )
    {
        if(numTokens==1)
        {
            cout << "testApp :: process TCP :: OSC MESSAGE WITH NO ARGUMENTS AT ALL ? " << tokens[0] << endl;
        }
        
        ///---------------------------------
        /// 1 TOKENS : 1 COMMAND NO PARAMS
        ///---------------------------------
        
        else if(numTokens==2)
        {
            // 1 token means just a command like : pause, saveQuad, resetQuad, nextQuadPoint, preQuadPoint, ping, shutdown, reboot, debug
            // so just pass though the command string
            ofLog(OF_LOG_NOTICE) << " 1 Argument  " << tokens[0] << " : " << tokens[1] << endl;
            cout << " 1 Argument  " << tokens[0] << " : " << tokens[1] << endl;
            /// (0) is the command
            /// ----------------------------
            myMessage->addStringArg(tokens[1]);
            
            if(tokens[1]=="ping")
            {
                cout << "Hi!! you ping I pong !!" << endl;
                tcpClient.send("pong " +id+ " "+ name);
            }
            else if(tokens[1]=="exit")
            {
                cout << "Hi!! I'm exit !!" << endl;
                ofLog(OF_LOG_NOTICE) << " osc received exit !! " << endl;
                this->exit();
                std::exit(0);
            }
            else if(tokens[1]=="shutdown")
            {
                cout << "Hi!! I'm shutdown !!" << endl;
                ofLog(OF_LOG_NOTICE) << " shutdown ? " << endl;
                string scriptPath = ofToDataPath("scripts/shutdown.sh", true);
                //ofSystem("/home/pi/openframeworks/apps/MIESPI/WarpPi_rev5/bin/data/scripts/shutdown.sh");
                ofSystem(scriptPath);
                
            }
            else if(tokens[1]=="reboot")
            {
                cout << "Hi!! I'm reboot !!" << endl;
                ofLog(OF_LOG_NOTICE) << " REBOOT ? " << endl;
                string scriptPath = ofToDataPath("scripts/reboot.sh", true);
//                ofSystem("/home/pi/openframeworks/apps/MIESPI/WarpPi_rev5/bin/data/scripts/reboot.sh");
                ofSystem(scriptPath);

            }
            else if(tokens[1] == "close")
            {
                cout << " received close connection" << endl;
                ofLog(OF_LOG_NOTICE) << " Close connection " << endl;
                tcpClient.close();
            }
        }
        
        ///---------------------------------
        /// 2 TOKENS : 1 COMMAND 1 PARAMETER
        ///---------------------------------
        
        else if(numTokens==3)
        {
            ofLog(OF_LOG_NOTICE) << " 2 Argument  " << tokens[0] << " : " << tokens[1] << " : " << tokens[2] << endl;
            cout << " 2 Argument  " << tokens[0] << " : " << tokens[1] << " : " << tokens[2] << endl;
            
            /// (0) is the command string
            /// ----------------------------
            
            myMessage->addStringArg(tokens[1]);
            
            if((tokens[1]=="editQuad")||(tokens[1]=="movePointUp")||(tokens[1]=="movePointDown")||(tokens[1]=="movePointRight")||(tokens[1]=="movePointLeft"))
            {
                /// 2 ARGUMENTS 0/ command string 1/int
                /// ----------------------------
                // (1) is an int on this commands
                int val = ofToInt(tokens[2]);
                myMessage->addIntArg(val);
            }
            else if ((tokens[1]=="playVideo")||(tokens[1]=="stopVideo")||(tokens[1]=="restartVideo")||(tokens[1]=="playImage")||(tokens[1]=="stopImage") || (tokens[1]=="playArtnet")||(tokens[1]=="stopArtnet")||(tokens[1]=="restartArtnet"))
            {
                /// 2 ARGUMENTS 0/ command string 1/float
                /// ----------------------------
                // (1) is an float on this commands
                float valF = ofToFloat(tokens[2]);
                myMessage->addFloatArg(valF);
                
            }
            else if((tokens[1]=="volume"))
            {
                float valF = ofToFloat(tokens[2]);
                myMessage->addFloatArg(valF);
            }
            else if((tokens[1]=="fbo") || (tokens[1]=="homography"))
            {
                float valF = ofToInt(tokens[2]);
                bool valB = (valF == 1) ? true : false;
                myMessage->addBoolArg(valB);
            }
            
            else if((tokens[1]=="test"))
            {
                int valF = ofToInt(tokens[2]);
                bool valB = (valF == 1) ? true : false;
                myMessage->addBoolArg(valB);
            }
            else if((tokens[1]=="debug"))
            {
                int valF = ofToInt(tokens[2]);
                bool valB = (valF == 1) ? true : false;
                isDebugging = !isDebugging;
                screen.isDebugging = !screen.isDebugging;
            }
            
        }
        
        ///---------------------------------
        /// 3 TOKENS : 1 COMMAND 2 PARAMETER
        ///---------------------------------
        
        else if(numTokens==4)
        {
            ofLog(OF_LOG_NOTICE) << " 3 Argument  " << tokens[0] << " : " << tokens[1] << " : " << tokens[2] << " : " << tokens[3] << " : "  << endl;
            
            /// (0) is the command string
            /// ----------------------------
            myMessage->addStringArg(tokens[1]);
            
            if((tokens[1]=="setAllDMXCh"))
            {
                /// 3 ARGUMENTS 0/ command string 1/int 2/float
                /// ----------------------------
                
                // (1) is an int on this commands
                int val = ofToInt(tokens[2]);
                myMessage->addIntArg(val);
                
                // (2) is an float on this commands
                float valF = ofToFloat(tokens[3]);
                myMessage->addFloatArg(valF);
                
            }
            else if((tokens[1]=="loadVideo") || (tokens[1]=="loadImage") || (tokens[1]=="loadFolder") || (tokens[1]=="loadArtnet"))
            {
                /// 3 ARGUMENTS 0/ command string /1 string 2/float
                /// ----------------------------
                
                // (1) is an int on this commands
                string val = ofToString(tokens[2]);
                myMessage->addStringArg(val);
                
                // (2) is an float on this commands
                float valF = ofToFloat(tokens[3]);
                myMessage->addFloatArg(valF);
            }
            
            
            
        }
        
        ///---------------------------------
        /// 4 TOKENS : 1 COMMAND 3 PARAMETER
        ///---------------------------------
        
        else if(numTokens==5)
        {
            ofLog(OF_LOG_NOTICE) << " 4 Argument  " << tokens[0] << " : " << tokens[1] << " : " << tokens[2] << " : " << tokens[3] << " : "<< tokens[4] << " : " << endl;
            
            /// (0) is the command string
            /// ----------------------------
            myMessage->addStringArg(tokens[1]);
            
            if((tokens[1]=="setDMXCh") || (tokens[1]=="setArtnetCh"))
            {
                /// 4 ARGUMENTS 0/ command string 1/int /2 int 3/float
                /// ----------------------------
                
                // (1) is an int on this commands
                int val = ofToInt(tokens[2]);
                myMessage->addIntArg(val);
                
                // (2) is an int on this commands
                int val2 = ofToInt(tokens[3]);
                myMessage->addIntArg(val2);
                
                // (3) is an float on this commands
                float valF = ofToFloat(tokens[4]);
                myMessage->addFloatArg(valF);
            }
            else if((tokens[1]=="loadFolder"))
            {
                /// 3 ARGUMENTS 0/ command string /1 string 2/float
                /// ----------------------------
                
                // (1) is an int on this commands
                string val = ofToString(tokens[2]);
                myMessage->addStringArg(val);
                
                // (2) is an float on this commands
                float valF = ofToFloat(tokens[3]);
                myMessage->addFloatArg(valF);
                
                float valF2 = ofToFloat(tokens[4]);
                myMessage->addFloatArg(valF2);
            }
            
            
        }
        
        //        cout << "testApp : process TCP :: myMessage is OSC address : @ " << myMessage->getAddress() << " : " << myMessage->getArgAsString(0) << " , " << myMessage->getArgAsInt32(1) << endl;
        
        
        return(myMessage);
    }
    
    return(myMessage);
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

    // BACKGROUND
    ///////////////
    ofBackground(0);

//	if(!useFbo)
//    {
//        // IF WE DON'T USE THE FBO -> NO HOMOGRAPHY WARPING -> DRAW THE VIDEO TEXTURE
//        ofSetColor(255);
//        #ifdef TARGET_RASPBERRY_PI
//        {
//            pmWarpPiRendererOMXPlayer* o = (pmWarpPiRendererOMXPlayer*) renderers[0];
//            o->drawPlayer();// omxPlayer->draw(0, 0, ofGetWidth(), ofGetHeight());
//        }
//        #endif
//        
//        #ifdef TARGET_OS_MAC
//        {
//            pmWarpPiRendererVideoPlayer* v = (pmWarpPiRendererVideoPlayer*) renderers[0];
//            v->drawPlayer();//->draw(0,0,ofGetWidth(),ofGetHeight());
//        }
//        #endif
//    }
//    else
//    {
        // IF WE USE FBO -> APPLY HOMOGRAPHY WARPING -> DRAW RENDERERS NORMALLY
    screen.draw();
    
    if(this->isDebugging)
    {
        showDebug();
    }
    if(showFPS)
    {
        ofSetColor(255);
        ofDrawBitmapString(ofToString(ofGetFrameRate()),ofGetWidth()-50,ofGetHeight()-50);
    }
		
}

//--------------------------------------------------------------
void ofApp::exit()
{
    ofLog(OF_LOG_NOTICE) << "on exit() :: TCP CLOSING !!! " << endl;
    cout <<  "on exit() :: TCP CLOSING !!! " << endl;
    sendTCPAwake();
    tcpClient.close();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    if (key=='f')
    {
        showFPS=!showFPS;
    }
    else if (key == 'd')
    {
        isDebugging = !isDebugging;
        screen.isDebugging = !screen.isDebugging;
    }
    else if (key=='o')
    {
        //useFbo = !useFbo;
        screen.useFbo = !screen.useFbo;
    }
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){}

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
    ofDrawBitmapString("IP : " + pmLocalAddressGrabber::getMyProbableIpAddress() ,debugPosition.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofDrawBitmapString("OSC RECEIVE PORT: "  + ofToString(confOscReceivePort),debugPosition.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofDrawBitmapString("OSC SEND TO ADDRESS : "  + ofToString(confOscSendIpAddress),debugPosition.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofDrawBitmapString("OSC SEND TO PORT : "  + ofToString(confOscSendPort),debugPosition.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofDrawBitmapString("LAST OSC MSG : "  + lastOscMessage,debugPosition.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofDrawBitmapString("USE TCP : "  + ofToString(confUsesTCP),debugPosition.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofDrawBitmapString("TCP CONNECTION : "  + ofToString(tcpAreWeConnected),debugPosition.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofDrawBitmapString("TCP IP : "  + ofToString(confTCPSendIpAddress),debugPosition.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofDrawBitmapString("TCP PORT : "  + ofToString(confTCPPort),debugPosition.x,whichHeight);
}


//--------------------------------------------------------------
void ofApp::sendTCPAwake()
{
    cout << "Sending TCP : awake to server !! " << confTCPSendIpAddress << " : " << confTCPPort << endl;
    tcpClient.send("awake "  + ofToString(confId));
}