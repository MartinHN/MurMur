//
//  BlobHandler.h
//  MurMur d'Ombre
//
//  Created by martin hermant on 17/02/14.
//
//

#pragma once


#include "Constants.h"

#include "EcranHandler.h"
#include "ofxOpenCv.h"

#include "ofMain.h"

#if USE_SYPHON
class ofxSyphonClient;
#elif USE_KINECT
class ofxKinect;
#elif USE_REMOTE_KINECT
class ofxOscReciever ;
#endif

class BlobHandler{
    
public:
    
    BlobHandler();
    ~BlobHandler();
    
    void setup(int inwin, int inhin,ScreenHandler * sH);
    
    
    //    void computePoly();
    const ofTexture & getDepthTex()const;
    void update();
    //    void blurblob();
    void updateDepthTex();
    void getGS();
    void compBlob();
    void registerParams();

    ofParameterGroup persistentGroup;
    void compCache();
    void setupData(ofShader* blurXin,ofShader * blurYin);
    vector<ofVec3f> compCentroid(float w=1,float h=1);
    vector<ofRectangle> compBounds(float w=1, float h=1);
    vector<ofVec3f> compExtrems(float w=1, float h=1);
    
    
    vector<ofPolyline> getBlobs(float w=1,float h=1,bool invx = false,bool invy = false);
    vector<ofPath> getPaths(float w=1,float h = 1,bool invx = false,bool invy = false);

    
    vector<ofVec3f> arms;
    vector<ofVec3f> centroids;
    vector<ofRectangle> boxes;

    int inw,inh;
    
#if USE_SYPHON
    std::unique_ptr<ofxSyphonClient> blobClient;
#elif USE_KINECT
    std::unique_ptr<ofxKinect> kinect;
#elif USE_REMOTE_KINECT
    std::unique_ptr<ofxOscReciever> oscRcv;
#endif


    ofFbo depthTex;


    ScreenHandler * sH;
    ofShader * blurX;
    ofShader * blurY;
    ofShader threshBW;
    ofxCvContourFinder contourFinder;
    ofPixels pix;

    ofxCvGrayscaleImage gs;
    ofxCvColorImage colorIm;
    
    
    vector<ofPolyline> cachedP,lcacheP;

    bool polyCacheDirty;
    int lastw,lasth,lastcw,lastch;
    
    vector<ofxCvBlob> blobs;

    ofParameter<float> vidThreshold,blobBlur;
    ofParameterGroup settings;
    ofParameter<float> minSide, maxSide,maxLengthExtrem,maxArmWidth;
    ofParameter<int> maxBlobs,polyMaxPoints;
    ofParameter<bool> computeBlob,findHoles,invertBW,invertX,invertY;
    ofParameter<float> simplification,smooth;
    ofParameter<ofVec4f> crop;
    ofParameter<int> screenN;
    ofParameter<ofVec3f> scale,pos;
    ofParameter<bool> isPiping;
};


