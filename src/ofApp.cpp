#include "ofApp.h"

#define VISU_OSC_IN  12344
#define VISU_OSC_OUT 12346

#define VISU_OSC_IN2 11000
#define VISU_OSC_OUT2 11001
#define SERVER_PORT 20000


#define VISU_OSC_IP_OUT "nano.local"
#define UI_IP "tintamar.local"
//#define VISU_OSC_IP_OUT "localhost"
//#define UI_IP "localhost"
#include "CustomOSCSync.hpp"

template <class T>
ofReadOnlyParameter<T,float> makeRO(ofParameter<T> & p){
    return ofReadOnlyParameter<T,float>(p);
}
#define SETPERSISTENT(x) {auto ro =makeRO(x); x.setSerializable(false);persistentGroup.add(ro);}


ofApp::ofApp(){

}



ofApp::~ofApp(){

}
//--------------------------------------------------------------
void ofApp::setup(){
    //    ofSetDataPathRoot("../Resources/data/");
    isFPS = false;
#ifndef GUIMODE





    ofSetLogLevel(OF_LOG_VERBOSE);

    isFullScreen=false;



    ofBackground(0);
    glDisable(GL_DEPTH_TEST);
    ofEnableAlphaBlending();
    glBlendEquation(GL_FUNC_ADD_EXT);
    glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
    ofSetBackgroundAuto(false);
    ofEnableArbTex();
    glEnable(GL_POINT_SPRITE);
    glPointParameteri(	GL_POINT_SPRITE_COORD_ORIGIN,GL_UPPER_LEFT);

    ofSetVerticalSync(true);
    ofSetFrameRate(FPS);

    width = ofGetWindowWidth();
    height = ofGetWindowHeight();

    inw=SYPHON_W;//320;
    inh=SYPHON_H;//240;


    //Zdepth
    zdepth=1000;


#ifdef testvid
    vidplay.loadMovie("bien.mov");
    vidplay.play();
    threshold=55;
#endif





    blurX.load("","shaders/blurX.frag");
    blurY.load("","shaders/blurY.frag");
    colorMod.load("","shaders/colorMod.frag");
    bloom.load("","shaders/bloom.frag");
    gloom.load("","shaders/gloom.frag");
    invertColorShader.load("","shaders/invertColor.frag");
    kaleidoscope.load("","shaders/kaleidoscope.frag");

    finalRender.allocate(scrw,scrh,GL_RGB);


#if defined  localcamera || defined testvid || defined blobcomp

    colorImg.allocate(inw,inh);
    grayImage.allocate(inw,inh);
    grayDiff.allocate(inw,inh);

    bLearnBakground=true;

#endif



    finalblur=0;



    attrctl = AttrCtl();


#endif
    sH.setup(&scrw,&scrh,zdepth);
    bH.setup(inw,inh,&sH);
    visuHandler.setup(&attrctl,&bH,inw,inh,zdepth,&scrw,&scrh,&sH);
    persistentSettingsPath = "persistentSetting.xml";
    
#ifndef GUIMODE



#ifdef blobosc
    drawBlob = false;
#endif


#endif
    camera2.setup(&scrw,&scrh, &zdepth);


    globalParam.setName("OF");
    settings.setName("global");
    MYPARAM(loadName, "", "", "");
    loadName.setSerializable(false);
    MYPARAM(saveName, "", "", "");
    saveName.setSerializable(false);
    saveName.addListener(this, &ofApp::saveState);
    loadName.addListener(this, &ofApp::loadState);
    //#ifdef GUIMODE
    //    MYPARAM(GUIRate, 5, 1, 10);
    //#endif
    MYPARAM(finalblur, 0.f, 0.f, 10.f);
    MYPARAM(saturation, 1.f, 0.f, 2.f);
    MYPARAM(contrast, 1.f, 0.f, 2.f);
    MYPARAM(brightness, 1.f, 0.f, 2.f);
    MYPARAM(brightnessM,100.f,0.f,100.f);
    brightnessM.setSerializable(false);
    MYPARAM(rback, 0, 0, 255);
    MYPARAM(gback, 0, 0, 255);
    MYPARAM(bback, 0, 0, 255);
    MYPARAM(alphablur, 255, 0, 255);
    MYPARAM(bloomsize,0,0,10);
    MYPARAM(isGloom,false,false,true);
    MYPARAM(gloomSize,0,0,10);
    MYPARAM(invertColor,false,false,true);
    MYPARAM(isKaleidoscope,false,false,true);
    MYPARAM(kaleidoscopeScaleX,0.1,0.1,1);
    MYPARAM(kaleidoscopeScaleY,0.1,0.1,1);
    MYPARAM(kaleidoscopeBlend,0,0,1);
    MYPARAM(kaleidoscopeAngle,0,0,6);
    MYPARAM(isPipe,false,false,true);
    MYPARAM(pipeblur, 0.f,0.f,25.f);
    MYPARAM(pipeAlphablur, 255,0,255);
    MYPARAM(hidePipe,false,false,true);
    MYPARAM(pipeMask,false,false,true);
    MYPARAM(cropScreen,ofVec4f(0,0,0,0),ofVec4f(0),ofVec4f(0.6))
    cropScreen.setSerializable(false);
    MYPARAM(fullScreen,false    ,true,false);
    fullScreen.setSerializable(false);
    fullScreen.addListener(this, &ofApp::askFullScreen);


    settings.add(camera2.settings);


    //    settings.add(finalblur);

    visuHandler.addVisu(new background(&visuHandler));

    visuHandler.addVisu(new Particles(&visuHandler));
    visuHandler.addVisu(new metaBalls(&visuHandler));
    visuHandler.addVisu(new AutoTree(&visuHandler));

    //    visuHandler.addVisu(new BallManager(&visuHandler));
    visuHandler.addVisu(new drawBlob(&visuHandler));

    //visuHandler.addVisu(new VideoPlayer(&visuHandler));

    //    visuHandler.addVisu(new Photo(&visuHandler));
   // visuHandler.addVisu(new ColorRuban(&visuHandler));
    visuHandler.addVisu(new Liner(&visuHandler));
    //    visuHandler.addVisu(new Baton(&visuHandler));

    visuHandler.addVisu(new boule2gomme(&visuHandler));


    visuHandler.registerParams();
    sH.registerParams();



    globalParam.add(settings);

    globalParam.add(bH.settings);
    globalParam.add(attrctl.settings);
    globalParam.add(sH.screensCtl);
    globalParam.add(sH.screensParam);
    globalParam.add(visuHandler.allParams);
    //    ofParameterGroup pg;
    //    screensParam.add(sH.screensParam);
    //    screensParam.setName("screensG");
    //    ofParameterGroup pg = screensParam.getGroup("screens");

    paramSync.reset(new CustomOSCSync());
#ifdef GUIMODE
//    ofxGuiSetFont(ofTrueTypeFontSettings(OF_TTF_SANS, 10));
    ofxGuiSetDefaultHeight(10);
    liveMode = false;

    paramSync->setup(globalParam,VISU_OSC_IN,VISU_OSC_IP_OUT,VISU_OSC_OUT);
    paramSync2.reset(new CustomOSCSync());
    paramSync2->setup(globalParam,VISU_OSC_IN2,"localhost",VISU_OSC_OUT2);
    clientServerReceiver = new ofxOscReceiver();
    clientServerReceiver->setup(SERVER_PORT);
    sH.setupUI();
#else
    visuHandler.setupData();
    bH.setupData(&blurX,&blurY);
    sH.setupData();
    paramSync->setup(globalParam,VISU_OSC_OUT,UI_IP,VISU_OSC_IN);


#endif

#ifdef GUIMODE
    //    ofSetFrameRate(60);
    //    GUIFBO.allocate(scrw,scrh,GL_RGB);

    ofSetFrameRate(60);
    gui.load(globalParam);


#endif

}




//--------------------------------------------------------------

///// UPDATE

//---------------------------------------------------------------



void ofApp::update(){



#ifdef GUIMODE
    clientServerUpdate();

#endif
    paramSync->update();
#ifndef GUIMODE

    bH.update();


    //    attrctl.clearPoints();

    attrctl.addPoints(bH.centroids, 1);
    attrctl.addPoints(bH.arms, 2);

    attrctl.update();


    visuHandler.update();



    for (int ncom = 0 ; ncom< computeRatio;ncom++){
        visuHandler.updateHighFPS();
    }








#endif


}


void ofApp::askFullScreen(bool & f){
#ifndef GUIMODE
    ofSetFullscreen(f);
    isFullScreen = f;
#endif
}

//--------------------------------------------------------------
//
//     DRAW
//
//--------------------------------------------------------------




void ofApp::draw(){

#ifdef GUIMODE
    ofSetBackgroundAuto(true);
    //    if(ofGetFrameNum()%GUIRate==0 || ofGetMousePressed()){
    //        ofDisableAlphaBlending();
    //        GUIFBO.begin();
    //        glBlendEquation(GL_ADD);
    //        glBlendFunc(GL_DST_COLOR,GL_ZERO);
    //        ofSetColor(80,80,80,255);
    //        ofRect(0,0,scrw,scrh);
    //        ofSetColor(255);
    //        gui.draw();
    //        GUIFBO.end();
    //    }
    //    GUIFBO.draw(0,0,scrw,scrh);
    if(!liveMode){
        gui.draw(scrw,scrh);
    }
    else{
        ofSetColor(0);
        ofRect(0,0,scrw,scrh);
    }

#else


    if(isPipe){
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        ofPushMatrix();
        ofPushView();


        visuHandler.pipePP.src->begin();

        ofSetColor(0,0,0,pipeAlphablur);
        ofRect(0,0,inw,inh);


        ofEnableAlphaBlending();
        ofSetColor(255,255,255,255);

        //    glBlendEquation(GL_FUNC_ADD);
        //    glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA);


        //Draw only for pipe

        visuHandler.draw(DrawPass::DrawPipe);




        visuHandler.pipePP.src->end();

        ofPopMatrix();
        ofPopView();

        //PIPE Blur
        glBlendEquation(GL_FUNC_ADD_EXT);
        glBlendFunc(GL_ONE,GL_ZERO);
        ofSetColor(255);
        if(pipeblur>0){
            visuHandler.pipePP.dst->begin();
            blurX.begin();
            blurX.setUniform1f("blurAmnt", pipeblur);
            visuHandler.pipePP.src->draw(0,0);
            blurX.end();
            visuHandler.pipePP.dst->end();
            ofSetColor(255);
            visuHandler.pipePP.swap();

            visuHandler.pipePP.dst->begin();
            blurY.begin();
            blurY.setUniform1f("blurAmnt", pipeblur);

            visuHandler.pipePP.src->draw(0,0);

            blurY.end();
            visuHandler.pipePP.dst->end();
            visuHandler.pipePP.swap();

        }
        //        glBlendEquation(GL_FUNC_ADD);
        //        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }




    //Direct Render

    //glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_DST_ALPHA, GL_DST_ALPHA);
    ofEnableAlphaBlending();
    finalRender.src->begin();
    //        if(hidePipe) {
    ofSetColor(rback,gback,bback,alphablur);
    //            if(!isPipe)ofRect(0,0,scrw,scrh);
    //            ofSetColor(0,0,0,255);
    ofRect(0,0,scrw,scrh);
    ofSetColor(255);
    //        }



    ofPushMatrix();
    ofPushView();
    ofEnableAlphaBlending();
    camera2.begin();
    //draw reciever of pipe
    visuHandler.draw(DrawPass::DrawFull);

    camera2.end();
    ofPopMatrix();
    ofPopView();

    finalRender.src->end();





    //FinalTOUCH
    glBlendEquation(GL_FUNC_ADD_EXT);
    glBlendFunc(GL_ONE,GL_ZERO);
    //

    if(finalblur>0){

        finalRender.dst->begin();
        blurX.begin();
        blurX.setUniform1f("blurAmnt",finalblur);
        finalRender.src->draw(0,0);
        blurX.end();
        finalRender.dst->end();

        finalRender.swap();

        finalRender.dst->begin();
        blurY.begin();
        blurY.setUniform1f("blurAmnt",finalblur);
        finalRender.src->draw(0,0);
        blurY.end();
        finalRender.dst->end();

        finalRender.swap();

    }


    finalRender.dst->begin();
    colorMod.begin();
    colorMod.setUniform1f("contrast",contrast);
    colorMod.setUniform1f("saturation",saturation);
    colorMod.setUniform1f("brightness",brightness*brightnessM/100.);
    finalRender.src->draw(0,0);

    colorMod.end();
    finalRender.dst->end();

    finalRender.swap();

    if(bloomsize>0){
        finalRender.dst->begin();
        bloom.begin();
        bloom.setUniform1i("size", bloomsize);
        finalRender.src->draw(0,0);
        bloom.end();
        finalRender.dst->end();

        finalRender.swap();
    }
    if(isGloom){
        finalRender.dst->begin();
        gloom.begin();
        gloom.setUniform1f("blurSize", gloomSize);
        finalRender.src->draw(0,0);
        gloom.end();
        finalRender.dst->end();

        finalRender.swap();

    }
    if(isKaleidoscope){
        finalRender.dst->begin();
        kaleidoscope.begin();
        kaleidoscope.setUniform2f("resolution",scrw,scrh);
        kaleidoscope.setUniform1f("angle",kaleidoscopeAngle);
        kaleidoscope.setUniform1f("scaleX",kaleidoscopeScaleX);
        kaleidoscope.setUniform1f("scaleY",kaleidoscopeScaleY);
        kaleidoscope.setUniform1f("blend",kaleidoscopeBlend);
        finalRender.src->draw(0,0);
        kaleidoscope.end();
        finalRender.dst->end();

        finalRender.swap();

    }
    if(invertColor){
        finalRender.dst->begin();
        invertColorShader.begin();
        finalRender.src->draw(0,0);
        invertColorShader.end();
        finalRender.dst->end();

        finalRender.swap();
    }


    ofSetColor(255);

    bool oneMask = false;
    for (int i = 0 ; i<visuHandler.visuList.size();i++){
        if(visuHandler.visuList[i]->isMasking && visuHandler.visuList[i]->isActive){
            oneMask=true;
            break;
        }
    }
    if(!oneMask){
        for(int i = 0 ; i< sH.screenL.size();i++){
            if(sH.screensCtl.getBool("mask"+ofToString(i))){
                oneMask=true;
                break;

            }
        }
    }

    oneMask |= pipeMask;

    oneMask |= cropScreen->x>0 ||
    cropScreen->y>0 ||
    cropScreen->z>0 ||
    cropScreen->w>0;

    if(oneMask){

//        glBlendEquation(GL_MIN);
//        glBlendFunc(GL_DST_COLOR,GL_ZERO);
//        glBlendEquation(GL_FUNC_ADD);

//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
        if(!sH.invertMask)ofSetColor(0);
        else
            ofSetColor(255);
        ofRect(0, 0, scrw, scrh);


        if(pipeMask){
            if(sH.invertMask)invertColorShader.begin();
            visuHandler.pipePP.src->draw(0,0,scrw,scrh);
            if(sH.invertMask)invertColorShader.end();
        }

        if(!sH.invertMask)ofSetColor(255);
        else ofSetColor(0);
        visuHandler.draw(DrawPass::DrawMask);

        sH.drawMask();
        glBlendEquation(GL_ADD);
        glBlendFunc(GL_DST_COLOR,GL_ZERO);


        ofSetColor(255);
        finalRender.src->draw(0,0,scrw,scrh);
        ofSetColor(0);
        if(cropScreen->x)
            ofRect(0,0,cropScreen->x*scrw,scrh);
        if(cropScreen->y)
            ofRect(0,0,scrw,cropScreen->y*scrh);
        if(cropScreen->z)
            ofRect(scrw*(1-cropScreen->z),0,scrw*cropScreen->z,scrh);
        if(cropScreen->w)
            ofRect(0,scrh*(1-cropScreen->w),scrw,scrh*cropScreen->w);
        
    }
    else{
        finalRender.src->draw(0,0,scrw,scrh);
//        ofSetColor(255);
//        ofRect(0, 0, scrw, scrh);
    }




    //    cout << ofGetFrameNum()<< endl;

#endif
    if(isFPS){
        ofSetColor(0,0,0,255);
        ofRect(0, 0, 300, 25);
        ofSetColor(255, 255, 255,255);
        ofDrawBitmapString("Fps: " + ofToString( ofGetFrameRate()) , 15,15);
    }
    else{
        //        ofSetColor(0,0,0);
        //        ofRect(0, 00, 1000, 25);
        //        ofSetColor(255, 255, 255);
        //        ofDrawBitmapString("Un Des Sens : www.undessens.org",50,40);
        //        ofDrawBitmapString(" www.facebook.com/AssoUnDesSens",500,40);
    }

}
#ifdef GUIMODE
void ofApp::keyPressed(int key){
    switch (key){
        case 's':
        {ofFileDialogResult filep = ofSystemSaveDialog("preset","save preset file");
            saveName = filep.getPath();
        }
            break;

        case 'l':
        {ofFileDialogResult filep = ofSystemLoadDialog("load preset");
            loadName = filep.getPath();

        }
            break;
        case 'r':
            if(ofFile::doesFileExist(std::string(loadName))){
                saveName = loadName;
            }
            break;


        case'v':
            gui.visuSettings++;
            gui.visuSettings %=2 ; // ignore screen
            break;
        
        case'2':
        case '1':
        case '0':
            gui.visuSettings = key - '0';
            break;

        case 'g':
            liveMode = !liveMode;
            ofSetWindowShape(liveMode?100:1000, liveMode?30:800);
            if(liveMode){gui.unload();}
            else{gui.load(globalParam,1000,800);}
            break;
            
//        case 'k':
//            savePersistent();
//            break;
//            
//        case 'i':
//            loadPersistent();
//            break;

        case 'f':
            isFPS=!isFPS ;
            break;

    }
}
#endif

void ofApp::savePersistent(){
    ofXml xml;
    ofSerialize(xml,bH.persistentGroup);
    xml.save(persistentSettingsPath);
    
}

void  ofApp::loadPersistent(){
    ofXml xml;
    xml.load(persistentSettingsPath);
    ofDeserialize(xml,bH.persistentGroup);
    
    
}
#ifndef GUIMODE
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    int k = 0;

    switch (key){


#ifdef blobcomp
        case 'b':
            bLearnBakground=true;
            break;
#endif

        case 's':
            isFullScreen=!isFullScreen;
            ofSetFullscreen(isFullScreen);
            break;




        case 356:
            camera2.ypr+=ofVec2f(5,0);
            break;

        case 357:
            camera2.ypr=ofVec2f(camera2.ypr.get().x,ofClamp(camera2.ypr.get().y+5,-90,90));
            break;

        case 358:
            camera2.ypr-=ofVec2f(5,0);
            break;

        case 359:
            camera2.ypr=ofVec2f(camera2.ypr.get().x,ofClamp(camera2.ypr.get().y-5,-90,90));
            break;

        case ' ':
#if defined testvid || defined localcamera
            vidplay.setPaused(!vidplay.isPaused());
#endif
            break;


        case 'f':
            isFPS=!isFPS ;
            break;
        case 'x':
            if( visuHandler.get("particles")){
                Particles * ptmp =(Particles *)visuHandler.get("particles") ;
                ptmp->initFbo();
            };
            break;
        case 'p':{
            static int imgCount(0);
            imgCount++;
            ofImage img;
            img.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
            img.saveImage("/tmp/murmur_"+ofToString(imgCount)+".jpg");
        }
            break;

        default :
            break;
    }



}









void ofApp::exit(){



}










void ofApp::mouseDragged(int x, int y, int button){
    vector<ofPoint> points;

    points.push_back(ofPoint(x*1.0/scrw,y*1.0/scrh,0));
    //    points.push_back(ofPoint(0.2,0.2,0));

    if(points.size()>0)attrctl.addPoints(points,0);

}
void ofApp::mouseReleased(int x, int y, int button){
    vector<ofPoint> points;

    points.clear();

    attrctl.addPoints(points,0);

}

#endif // ndef GUIMODE


//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
#ifdef GUIMODE
    scrw=w;
    scrh=h;
    //    GUIFBO.allocate(scrw,scrh,GL_RGB);
#else
#ifndef syphonout
    scrw=width = w;
    scrh=height=h;





    visuHandler.updateScreenSize();
    finalRender.allocate(width,height,GL_RGB);
    camera2.updateScreenSize(w, h);
#endif
#endif
}


#ifdef GUIMODE
void ofApp::clientServerUpdate(){
    ofxOscMessage m;
    //    /// return the address
    //    string getAddress() const { return address; }
    //
    //    /// return the remote ip
    //    string getRemoteIp() const { return remote_host; }
    //    /// return the remote port
    //    int getRemotePort() const { return remote_port; }
    if(clientServerReceiver){
        while(clientServerReceiver->getNextMessage(&m)){
            if(m.getAddress() == "/addMe"){
                auto ip = m.getRemoteIp();
                auto port = m.getRemotePort();

                ofLog() << ip +":"+ofToString(port);


                    paramSync2.reset( new CustomOSCSync());
//                    delete clientServerReceiver;
//                    clientServerReceiver = NULL;
                    paramSync2->setup(globalParam,VISU_OSC_IN2,ip,port);
//                    break;
            }
        }
    }
    if(paramSync2){
        paramSync2->update();
    }

}

#endif


void ofApp::saveState(string & s){
#ifdef GUIMODE
    if(s!=""){
        string abspath = ofToDataPath("presets/filage/"+ofToString(s));
        if(s.find("/")!=string::npos) {abspath = s;}
        else{
            ofLogWarning("saving to local : " + abspath);
        }
        cout<<"saving to " + abspath<<endl;
        ofXml xml;
        sH.screensParam.setSerializable(false);
        ofSerialize(xml,globalParam);
        sH.screensParam.setSerializable(true);
        cout<<xml.save(abspath)<<endl;
    }
    else{
        ofLogWarning("no argument for save state");
    }
#endif
}





void ofApp::loadState(string & s){
    //#if defined GUIMODE || defined STANDALONEMODE
    string abspath = "";
    if(s=="")return;
#ifdef GUIMODE
    if(s.find("/")!=string::npos){abspath = s;}
    else{
        abspath = ofToDataPath("presets/filage/"+ofToString(s),true);
        loadName = abspath; // will not retrigger loadState
    }
#else
    if(s.find("/")==string::npos){ abspath = ofToDataPath("presets/filage/"+ofToString(s),true);}
    else ofLogWarning("wrong preset name");
#endif
    
    if(abspath!=""){
        
        ofXml xml;
        
        xml.load(abspath);
        
        sH.screensParam.setSerializable(false);
        ofDeserialize(xml,globalParam);
        sH.screensParam.setSerializable(true);
    }
    else{
        ofLogWarning("no argument for load state");
    }
    //#endif  
}

