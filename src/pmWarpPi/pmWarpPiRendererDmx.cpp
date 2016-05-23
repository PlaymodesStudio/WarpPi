#include "pmWarpPiRendererDmx.h"


//-------------------------------------------------------------------------
pmWarpPiRendererDmx::pmWarpPiRendererDmx()
{
    dmx = new ofxDmx();
    ofLog(OF_LOG_NOTICE) << "pmDMX :: new Renderer DMX";
    type = renderDmx;
}

//-------------------------------------------------------------------------
void pmWarpPiRendererDmx::setupDmx(int _numDevice,int _numChannels,int _firstChannel)
{
    isTesting = false;
    dmxDebugPosition = ofVec2f(ofGetWidth()/2 + 20,20);
    testValue = 1.0;
    
 
    /// TWEENZOR must call this before adding any tweens
	Tweenzor::init();
//    Tweenzor::add((float *)&screenOpacity.get(), 0.0, 1.0, 0.0, 4,EASE_IN_OUT_EXPO);
//    Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&screenOpacity.get()), this, &pmWarpPiRendererScreen::onComplete);

    /// DMX
    firstChannel = _firstChannel;
    numDevice = _numDevice;
    numChannels = _numChannels;
    //dmx.connect("tty.usbserial-EN143965"); // use the name
	dmx->connect(numDevice); // or use a number

    /// GUI & Params
    gui->setup("channels"); // most of the time you don't need a name but don't forget to call setup
    gui->setPosition(dmxDebugPosition.x,dmxDebugPosition.y+60);
    for(int i=0;i<numChannels;i++)
    {
//        ofParameter<float>* prmtr = new ofParameter<float>();
        ofParameter<float> p;
        channels.push_back(p);
        gui->add(channels[i].set( "ch." +ofToString(i+1), 1.0, 0,1));
    }
    
    ofLog(OF_LOG_NOTICE) << "RendDmx :: setupDMX :: Device " << numDevice << " :: Num.Ch " << numChannels;
}

//-------------------------------------------------------------------------
void pmWarpPiRendererDmx::update(ofEventArgs & a)
{
    if(isTesting)
    {
        testDmx();
    }
    else
    {
        /// TWEENZOR
        Tweenzor::update( ofGetElapsedTimeMillis() );
        
        // all "tweened" values needs to be reassigned here !!
        for(int i=0;i<channels.size();i++)
        {
            channels[i] = channels[i] ;
            dmx->setLevel(firstChannel+i,channels[i] *255);
        }
        dmx->update();

        /*
         dmx.setLevel(11, chan1);
         dmx.setLevel(12, chan2);
         dmx.setLevel(13, chan3);
         dmx.update();
         */
    }
    
}


//-------------------------------------------------------------------------
void pmWarpPiRendererDmx::updateOSC(ofxOscMessage* m)
{
    
    ofLog(OF_LOG_NOTICE) << "RendDmx :: updateOSC " << m->getAddress();
    
    string address = m->getAddress();
    
    
    // get the id
    string addressWithoutSlash = address.substr(1,address.size()-1);
    
    if((address=="/all")||(id==addressWithoutSlash))
    {
        /// THIS MESSAGE IF FOR YOU !!
        
        /// COMMAND
        string command = m->getArgAsString(0);
        
        /// SET ALL DMX CHANNELS
        if(command == "setAllDMXCh")
        {
            int dmxValue        = m->getArgAsInt32(1);
            float timeToValue   = m->getArgAsFloat(2);
            
            for(int i=0;i<channels.size();i++)
            {
                //static void add(vector <float *> a_properties, float a_begin, float a_end, float a_delay, float a_duration, int a_easeType=EASE_LINEAR, float a_p=0, float a_a=0);
                Tweenzor::add((float *)&channels[i].get(), channels[i].get(), float(dmxValue)/255.0, 0.0, timeToValue,EASE_IN_OUT_EXPO);
                Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&channels[i].get()), this, &pmWarpPiRendererDmx::onComplete);
            }
        }
        /// SET ONE DMX CHANNEL
        else if(command == "setDMXCh")
        {
            
            int whichCh         = m->getArgAsInt32(1)-1;
            
            if((whichCh>=0) && (whichCh<channels.size()))
            {
                int dmxValue        = m->getArgAsInt32(2);
                float timeToValue   = m->getArgAsFloat(3);

                Tweenzor::add((float *)&channels[whichCh].get(), channels[whichCh].get(), float(dmxValue)/255.0, 0.0, timeToValue,EASE_IN_OUT_EXPO);
                Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&channels[whichCh].get()), this, &pmWarpPiRendererDmx::onComplete);
            }
        }

    }
}
//--------------------------------------------------------------
void pmWarpPiRendererDmx::onComplete(float* arg)
{

}


//-------------------------------------------------------------------------
void pmWarpPiRendererDmx::draw()
{
    if(isDebugging)
    {
        ofSetColor(255);
        showDmxDebug();
        
        // draw gui
        gui->draw();
    }
}

//-------------------------------------------------------------------------
void pmWarpPiRendererDmx::deleteRenderer()
{
    ofLog(OF_LOG_NOTICE) << "RendDmx:: delete DMX renderer";
    Tweenzor::destroy();

}

//-------------------------------------------------------------------------
void pmWarpPiRendererDmx::testDmx()
{
    // generate a sinus value to test dmx
    testValue = sin(ofGetElapsedTimef());
    testValue = testValue * testValue;
    
    for(int i=0;i<channels.size();i++)
    {
        channels[i] = fmod(testValue + (1.0/float(numChannels))*i,1.0);
        // all "tweened" values needs to be reassigned here !!
        channels[i] = channels[i] ;
        dmx->setLevel(firstChannel + i,int(channels[i]*255));
    }
    dmx->update();

    
}

//-------------------------------------------------------------------------
void pmWarpPiRendererDmx::showDmxDebug()
{
    int lineHeight = 15;
    int whichHeight = dmxDebugPosition.y;
    
    ofSetColor(255,128,0);
    ofDrawBitmapString("DMX",dmxDebugPosition.x,whichHeight);
    ofSetColor(255);
    whichHeight=whichHeight + lineHeight;
    ofDrawBitmapString("dmx serial device : " +ofToString(numDevice),dmxDebugPosition.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofDrawBitmapString("dmx num channels : " +ofToString(numChannels),dmxDebugPosition.x,whichHeight);
    whichHeight=whichHeight + lineHeight;
    ofDrawBitmapString("first channel : " +ofToString(firstChannel),dmxDebugPosition.x,whichHeight);
    
}

//-------------------------------------------------------------------------
void pmWarpPiRendererDmx::setChannel(int _ch,int _v)
{
    channels[_ch] = _v;
    channels[_ch] = channels[_ch] ;
    dmx->setLevel(firstChannel + _ch,_v);
    dmx->update();
}

//-------------------------------------------------------------------------
void pmWarpPiRendererDmx::setAllChannelsTo(int _v)
{
    for(int i=0;i<numChannels;i++)
    {
        channels[i] = _v;
        channels[i] = channels[i] ;
        dmx->setLevel(firstChannel + i,_v);
    }
    dmx->update();

}

//-------------------------------------------------------------------------
void pmWarpPiRendererDmx::setIsTesting(bool b)
{
    isTesting=b;

    if(isTesting==false)
    {
        /// AFTER TESTING SET ALL LIGHT TO 1
        for(int i=0;i<channels.size();i++)
        {
            //        add(vector <float *> a_properties, float a_begin, float a_end, float a_delay, float a_duration, int a_easeType=EASE_LINEAR, float a_p=0, float a_a=0);
            Tweenzor::add((float *)&channels[i].get(), channels[i].get(), 0.0f, 0.0f, 1.0f,EASE_IN_OUT_EXPO);
            Tweenzor::addCompleteListener( Tweenzor::getTween((float*)&channels[i].get()), this, &pmWarpPiRendererDmx::onComplete);
        }
    }
}

//-------------------------------------------------------------------------
void pmWarpPiRendererDmx::setIsDebugging(bool b)
{
    isDebugging=b;
}
