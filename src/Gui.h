//
//  Gui.h
//  MurMur d'Ombre
//
//  Created by Tinmar on 07/02/14.
//  Copyright (c) 2014 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofxGui.h"
#include "ofMain.h"
#include "Constants.h"



class Gui{
    
    
public:
    Gui():baseC(0){};
    ~Gui(){};
//    Gui(ofParameterGroup * p);

    void draw(int w, int h);
    void load(ofParameterGroup & pg,int targetW=0,int targetH=0);
    void unload();

//    ofxPanel glob;
//    ofParameterGroup globP;
    list<ofxPanel *> guiParam,guiVisu,guiScreen;

     int baseC ;
    int visuSettings;
    
    
};



