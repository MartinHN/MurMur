//
//  Baton.h
//  MurMur d'Ombre
//
//  Created by Tinmar on 14/01/14.
//  Copyright 2014 __MyCompanyName__. All rights reserved.
//
#pragma once

#include "Constants.h"
#if USE_CUSTOM_VISUX
#include "ofMain.h"
#include "VisuHandler.h"
#include "ofxOsc.h"

class Baton : public VisuClass{
public:
//    Baton(){};
    Baton(VisuHandler * dad);
    ~Baton(){};
    void drawFbo(int w,int h);
    
    
    
    //Herited method
    void setup();
    void update(int w, int h) override;
    void draw(int w, int h)override;

    
    
    
    
   
    
    vector<AttrStruct>* attr;
    ofImage stain;
    vector<ofPoint> stainl;
    int stainw,stainh;
    bool isNewBeat,noBeat;
    
    vector<ofPoint> poly;
    vector<ofPoint> poly2;
    
    
    ofPath polyline,polyline2;
    
    
    vector<float> orientation;
    int curthickness;
    ofParameter<int> maxsize;
    ofParameter<int> thickness;
    ofParameter<int> alpha;
    ofParameter<int> mode;
    ofParameter<ofVec3f> velocity;
    


    

    
private:
    int beat;
    float env;
    
};
#endif
