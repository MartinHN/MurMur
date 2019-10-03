#pragma once



#define CAMCOLOR

//#define STANDALONEMODE
//#define Debugcam
//#define localcamera
//#define testvid
//#define emulateshadow


//#define blobcomp

//#define syphonout

//#define isVisu





//#define blobosc

#include "ofMain.h"
#ifndef GUIMODE
//#include "ofxOpenCv.h"
#endif
#include "ofxNetwork.h"
#include "ofxOsc.h"




//#include "CamWiggler.h"

//#include "ofxXmlSettings.h"
//#include "ofx3DModelLoader.h"

//#include "ofxAssimpModelLoader.h"
//#include "ofxThreadedImageLoader.h"
//#include "VisuHandler.h"


#include "AttrCtl.h"



#include "Constants.h"
#include "CamHandler.h"
#ifdef GUIMODE
#include "Gui.h"
#include "ofxGui.h"
#endif
#include "visuWarper.h"

class CustomOSCSync;



class ofApp : public ofBaseApp{
public:
    ofApp();
    ~ofApp();
    void setup();
    void update();
    void draw();
    void saveState(string & s);
    void loadState(string & s);
    
   
     void keyPressed  (int key);

    void windowResized(int w, int h);

#ifndef GUIMODE
    
    void exit();    

    void mouseDragged(int x, int y, int button);
    void mouseReleased(int x, int y, int button);

    
    
    //Video INPUTS


    pingPongBuffer finalRender;

#if defined testvid || localcamera
    ofVideoGrabber vidGrab;
    ofVideoPlayer vidplay;
#endif

    
    int     width, height;


    bool iscam;


    
    
    //Camera
    ofVec4f camrot;
    ofVec4f camdest;
    float alphacamrot;
    
    int camctl;

    
    //BLOB
#if  defined blobcomp || defined testvid
    ofxCvColorImage colorImg;
    ofxCvGrayscaleImage 	grayImage;
    ofxCvGrayscaleImage 	grayBg;
    ofxCvGrayscaleImage 	grayDiff;
     Boolean bLearnBakground;
#endif


    
    ofShader blurX;
    ofShader blurY;
    ofShader colorMod;
    ofShader bloom;
    ofShader gloom;    
    ofShader invertColorShader,kaleidoscope;
    bool isFullScreen;
    
 
#endif // END NON GUIMOD

    
    CamHandler camera2;
   
    
    ofParameterGroup globalParam;    
    ofParameterGroup settings;
    ofParameter<float> finalblur;
    ofParameter<float> brightness,brightnessM,saturation,contrast;
    ofParameter<int> rback,bback,gback;
    ofParameter<int> alphablur;
    ofParameter<bool> sendToRemote;
    ofParameter<float> brightness2,saturation2,contrast2;
    ofParameter<string> saveName;
    ofParameter<string> loadName;
    ofParameter<bool> fullScreen;
    ofParameter<bool> isGloom,invertColor,isKaleidoscope;
    ofParameter<float>gloomSize;
    ofParameter<int> bloomsize,pipeAlphablur;
    
    ofParameter<bool> isPipe,hidePipe,pipeMask;
    ofParameter<float> pipeblur,kaleidoscopeScaleX,kaleidoscopeScaleY,kaleidoscopeBlend,kaleidoscopeAngle;
    ofParameter<ofVec4f> cropScreen;
    
    
    string persistentSettingsPath;
    ofParameterGroup persistentGroup;
    void  loadPersistent();
    void savePersistent();

    void askFullScreen(bool & f);
    int zdepth;
    int inw;
    int inh;
    VisuHandler visuHandler;
    

    BlobHandler bH;
    AttrCtl attrctl;
    
    unique_ptr<CustomOSCSync> paramSync;
#ifdef GUIMODE
    //OSC
    ofxOscReceiver*  clientServerReceiver; // used for external controller feedback
    void clientServerUpdate();
    unique_ptr<CustomOSCSync> paramSync2;
#endif

    bool isFPS;
//    unique_ptr<CustomOSCSync> screenSync;
    ofParameterGroup screensParam;
    
    void targetOSCChanged(bool & b);
    ScreenHandler sH;
    
    

#ifdef GUIMODE
    Gui gui;
    string lastpath;
    bool liveMode;
//    ofParameter<int> GUIRate ;
//    ofFbo GUIFBO;
#endif
    

};
