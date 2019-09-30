//
//  Liner.cpp
//  MurMur d'Ombre
//
//  Created by Tinmar on 04/02/14.
//  Copyright (c) 2014 __MyCompanyName__. All rights reserved.
//

#include "Liner.h"



Liner::Liner(VisuHandler * v):VisuClass(v){
    
    MYPARAM(width,2.f,0.f,10.f);
    MYPARAM(steps,10,1,100);
    MYPARAM(drawCentr, true,false,true);
    MYPARAM(rotAngle,0.f,0.f,5.f);
    MYPARAM(color,ofVec3f(255),ofVec3f(0),ofVec3f(255));
    MYPARAM(alphaColor,0,0,255);
    settings.setName("Liner");
}


void Liner::update(int w, int h){

    
}

void Liner::draw(int w, int h){

    
    
    vector<ofPolyline> b = dad->bH->getBlobs(w,h);
    ofSetColor(color->x,color->y,color->z,alphaColor);
    ofSetLineWidth(width);
    for (int i = 0 ; i < b.size(); i ++){
        ofPushMatrix();
        ofPoint centr = b[i].getCentroid2D();
        ofTranslate(centr.x,0);
        
        for (int j = 0 ; j < b[i].size() ; j++){
            
            ofRotateY(360.0*rotAngle/b[i].size());
            
            ofDrawLine(b[i][j]-ofVec3f(centr.x,0),b[i][(j+steps)%(b[i].size())]-ofVec3f(centr.x,0,0));
            
            if(drawCentr)ofDrawLine(b[i][j]-ofVec3f(centr.x,0),ofVec3f(0,centr.y));
        }
        ofPopMatrix();
    }

}
