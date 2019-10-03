//
//  ParticlesHandler.cpp
//  MurMur d'Ombre
//
//  Created by Tinmar on 14/08/13.
//  Copyright 2013 __MyCompanyName__. All rights reserved.
//

#include "ParticlesHandler.h"

Force::Force(){
    settings = ofParameterGroup();
    settings.setName("empty");
    isActive = true;
    
}
Force::Force(string namein,bool isAttr){
    name = namein;
#ifndef GUIMODE
    shader.load("","shaders/"+name+".frag");
#endif
    settings = ofParameterGroup();
    settings.setName(name);
    pl.push_back(&isActive);
    isActive.setName("isActive");
    settings.add(isActive);
    isActive = false;
    
    attrFamilly = -2;
    if(isAttr) {
        MYPARAM(attrFamilly,0,-1,13);
        MYPARAM(attrZone,0,0,3);
        pl.push_back(&attrFamilly);
        pl.push_back(&attrZone);

    }
    
}





ofShader Force::getShader(){
    return shader;
}


void Force::updateShader(){

    for (int i = 0 ; i < settings.size();i++){
        ofAbstractParameter *p = &settings[i];
        if(p->type()==typeid(ofParameter<int>).name())
            shader.setUniform1i(p->getName(),p->cast<int>());
        else if(p->type()==typeid(ofParameter<float>).name())
            shader.setUniform1f(p->getName(),p->cast<float>());
        else if(p->type()==typeid(ofParameter<ofVec3f>).name())
            shader.setUniform3f(p->getName(),p->cast<ofVec3f>().get()[0],p->cast<ofVec3f>().get()[1],p->cast<ofVec3f>().get()[2]);


    }
}



////////////////////////////////////////
//
//PArticles
//
//////////////////////////////////////////////////





Particles::Particles(VisuHandler * v):VisuClass(v){
    setup();
    isHighFPS = true;
    lastOriType  = -1;


}

void Particles::setupData(){

    origintype.addListener(this,&Particles::changeOrigins);
    gradNum.addListener(this,&Particles::changeGrad);
    numParticles.addListener(this,&Particles::changeNum);
    
    initFbo();
    
    updateRender.load("shaders/render.vert","shaders/render.frag");
    updatePos.load("","shaders/posUpdate.frag");
    int defaultgrad=0;
    changeGrad(defaultgrad);
    reset.addListener(this,&Particles::resetOrigins);

}
void Particles::setup(){

    settings.setName("Particles");
    
    
    
    
    noReset = false;

#ifdef PIMG
    int partRes = 100 ;
    //    partImg.allocate(partRes,partRes,OF_IMAGE_COLOR_ALPHA);
    ofVec2f center(partRes/2);
    //    for(int i = 0 ; i<partRes;i++){
    //        for(int j = 0 ; j<partRes;j++){
    //            ofVec2f curp(i,j);
    //            ofColor cc(0,0,255*max(0.,1-curp.distance(center)*1.0/partRes),255);
    //            partImg.setColor(i,j,cc);
    //        }
    //    }

    partImg.loadImage("photos/fire.jpg");

#endif


    //GLOBAL
    
    
    settings.setName("particles");
    
    
    MYPARAM(timeStep , 1.f,0.f,30.f);
    MYPARAM(timeStepM , 1.f,0.f,1.f);
    MYPARAM(is2D , false,false,true);
    timeStepM.setSerializable(false);
    
    MYPARAM(numParticles , 100000,100,1000000);
    lastnumParticles = numParticles;
    
    //Appearence
    MYPARAM(particleSize, 3.0f,0.f,30.f);
    MYPARAM(alphaColor,255,0,255);
    MYPARAM(alphaColorM,1.0f,0.0f,1.0f);
    alphaColorM.setSerializable(false);
    MYPARAM(color, ofVec3f(0,0,1.),ofVec3f(0.),ofVec3f(255.));
    MYPARAM(gradtype , 2,0,3);
    MYPARAM(gradNum,0,0,20);
    MYPARAM(mingrad,0.f,-1.f,1.f);
    MYPARAM(maxgrad,1,-1.f,3.f);
    MYPARAM(map2blob,true,false,true);
    MYPARAM(reset,false,false,true);


#ifdef PMOD
    MYPARAM(origintype,1,0,2);

#endif

    forces.push_back(new Force("originDeltas"));
    forces[forces.size()-1]->addParameter("k",.030f,0.f,.2f);
    forces[forces.size()-1]->addParameter("stretch",ofVec3f(1,1,1),ofVec3f(0),ofVec3f(4));
    forces[forces.size()-1]->addParameter("z",.030f,0.f,2.2f);

    forces.push_back(new Force("netw"));
    forces[forces.size()-1]->addParameter("k",.030f,0.f,.2f);
    forces[forces.size()-1]->addParameter("l0",1.0f,0.f,20.0f);
    forces[forces.size()-1]->addParameter("l0max",1.0f,0.f,20.0f);
    forces[forces.size()-1]->addParameter("z",.0f,0.f,2.f);
    forces[forces.size()-1]->addParameter("size",2,1,10);
    
    forces.push_back(new Force("neth"));
    forces[forces.size()-1]->addParameter("k",.030f,0.f,.2f);
    forces[forces.size()-1]->addParameter("l0",1.0f,0.f,20.f);
    forces[forces.size()-1]->addParameter("l0max",1.0f,0.f,20.0f);
    forces[forces.size()-1]->addParameter("z",.0f,0.f,2.f);
    forces[forces.size()-1]->addParameter("size",2,1,10);
    
    forces.push_back(new Force("origin"));
    forces[forces.size()-1]->addParameter("k",.10f,0.f,.25f);
    forces[forces.size()-1]->addParameter("z",.01f,0.f,.5f);
    forces[forces.size()-1]->addParameter("freeze",0.f,0.f,.80f);
    forces[forces.size()-1]->addParameter("freezemin",0.f,0.f,.30f);
    forces[forces.size()-1]->addParameter("damp",0.9f,0.5f,1.0f);
    forces[forces.size()-1]->addParameter("noiseFreq",4.f,0.f,50.f);
    forces[forces.size()-1]->addParameter("noiseAmp",0.f,0.f,1.f);
    forces[forces.size()-1]->addParameter("noisePhase",0.f,0.f,1.f);
    forces[forces.size()-1]->addParameter("applyOnBorder",0.f,0.f,1.f);
    
    forces.push_back(new Force("rotation",true));
    forces[forces.size()-1]->addParameter("type",0,0,1);
    forces[forces.size()-1]->addParameter("r",0.2f,0.f,.5f);
    forces[forces.size()-1]->addParameter("speed",0.f,0.f,0.5f);
    forces[forces.size()-1]->addParameter("orientation",ofVec3f(0,1,0),ofVec3f(0),ofVec3f(1));
    
    forces.push_back(new Force("gravity",true));
    forces[forces.size()-1]->addParameter("r",.50f,0.f,.5f);
    forces[forces.size()-1]->addParameter("mass",.001f,-.0002f,.02f);
    forces[forces.size()-1]->addParameter("rin",.50f,0.f,.5f);
    forces[forces.size()-1]->addParameter("massin",.00f,-.0002f,.0002f);
    forces[forces.size()-1]->addParameter("damp",1.f,.60f,1.0f);
    forces[forces.size()-1]->addParameter("distortX",1.f,0.f,2.f);

    forces.push_back(new Force("planeForce"));
    forces[forces.size()-1]->addParameter("k",.10f,0.f,.25f);
    forces[forces.size()-1]->addParameter("z",.01f,0.f,.5f);
    forces[forces.size()-1]->addParameter("freeze",0.f,0.f,.80f);
    forces[forces.size()-1]->addParameter("freezemin",0.f,0.f,.30f);
    forces[forces.size()-1]->addParameter("damp",0.9f,0.5f,1.0f);
    forces[forces.size()-1]->addParameter("noiseFreq",4.f,0.f,50.f);
    forces[forces.size()-1]->addParameter("noiseAmp",0.f,0.f,1.f);
    forces[forces.size()-1]->addParameter("noisePhase",0.f,0.f,1.f);

    
    forces.push_back(new Force("spring",true));
    forces[forces.size()-1]->addParameter("k",.030f,-.2f,1.f);
    forces[forces.size()-1]->addParameter("l0",.010f,0.f,.4f);
    forces[forces.size()-1]->addParameter("z",.0f,-.5f,.5f);
    forces[forces.size()-1]->addParameter("mode",1,0,1);
    forces[forces.size()-1]->addParameter("distortX",1.f,0.f,2.f);
    
    forces.push_back(new Force("fieldForce"));
    forces[forces.size()-1]->addParameter("k",.030f,-.3f,.3f);
    forces[forces.size()-1]->addParameter("velouty",.030f,-.1f,.1f);
    forces[forces.size()-1]->addParameter("veloutz",.030f,-.1f,.1f);
    forces[forces.size()-1]->addParameter("veloutborder",.00f,-.1f,.1f);
    forces[forces.size()-1]->addParameter("minv",.10f,0.f,1.f);
    forces[forces.size()-1]->addParameter("maxv",.80f,0.f,1.f);
    forces[forces.size()-1]->addParameter("fin",1.f,0.5f,1.3f);
    forces[forces.size()-1]->addParameter("fout",1.f,0.5f,1.3f);
    
    
    forces.push_back(new Force("globalForce"));
    forces[forces.size()-1]->addParameter("f",1.f,0.3f,1.f);
    forces[forces.size()-1]->addParameter("fmin",1.f,0.f,1.3f);
    forces[forces.size()-1]->addParameter("vmax",1.0f,0.f,1.f);
    forces[forces.size()-1]->addParameter("vmin",.0f,0.f,1.f);
    forces[forces.size()-1]->addParameter("comp",.0f,0.f,1.f);


    
    

    


    

}








void Particles::update(int w, int h){
    
    if(w!=lastw||h!=lasth){
        initFbo(w, h);
    }


    ofDisableAlphaBlending();
    glBlendEquation(GL_ADD);
    glBlendFunc(GL_ONE, GL_ZERO);
    
    for(int i = 0 ; i < forces.size();i++){
        if(forces[i]->isActive&&forces[i]->name.find("net")!=string::npos){

            velPingPong.dst->begin();
            forces[i]->shader.begin();
            forces[i]->shader.setUniformTexture("posData",posPingPong.src->getTextureReference(), 1);
            forces[i]->shader.setUniform3f("screen",w,h,dad->zdepth);
            forces[i]->shader.setUniform1i("resolution",textureRes);
            forces[i]->updateShader();
            
            velPingPong.src->draw(0,0);
            
            forces[i]->shader.end();
            
            velPingPong.dst->end();
            velPingPong.swap();



            //            }
        }
        else if(forces[i]->isActive ){
            int j = 0;
            vector<ofPoint> curattr = dad->attr->getType(forces[i]->attrFamilly,1,1,forces[i]->attrZone);
            if(map2blob){
                //                drawBlob * vvv = (drawBlob *)dad->get("drawBlob");
                //                for(int i = 0 ; i< curattr.size();i++){
                //                    if(vvv->invertx)curattr[i].x = 1-curattr[i].x;
                //                    if(vvv->inverty)curattr[i].y = 1-curattr[i].y;

                //
                //                }
                //            dad->sH.mapN2S(curattr,vvv->screenN);
                
                
            }
            if((forces[i]->attrFamilly>-2?curattr.size()>0:1)){
                do{
                    velPingPong.dst->begin();
                    forces[i]->shader.begin();
                    forces[i]->shader.setUniformTexture("posData",posPingPong.src->getTextureReference(), 1);
                    if(forces[i]->name=="origin")
                        forces[i]->shader.setUniformTexture("originData",origins.getTextureReference(), 2);
                    if(forces[i]->name=="originDeltas"){
                        forces[i]->shader.setUniformTexture("originDeltasLR",originLR.getTextureReference(), 2);
                        forces[i]->shader.setUniformTexture("originDeltasHB",originHB.getTextureReference(), 3);
                    }
                    if(forces[i]->name=="fieldForce"){
                        forces[i]->shader.setUniform2f("inres",dad->inw,dad->inh);
#ifdef syphon
                        forces[i]->shader.setUniformTexture("fieldData",dad->pipePP.src->getTextureReference(), 2);
#endif
                    }
                    if(forces[i]->attrFamilly>=0&&curattr.size()>0&&j<curattr.size()){
                        
                        forces[i]->shader.setUniform3f("attr",curattr[j].x,curattr[j].y,curattr[j].z+0.5);
                    }
                    forces[i]->shader.setUniform3f("screen",w,h,dad->zdepth);
                    if(origintype==1)forces[i]->shader.setUniform1i("resolution",textureRes3);
                    else forces[i]->shader.setUniform1i("resolution",textureRes);
                    forces[i]->updateShader();
                    
                    velPingPong.src->draw(0,0);
                    
                    forces[i]->shader.end();
                    
                    velPingPong.dst->end();
                    velPingPong.swap();
                    j++;
                }while( forces[i]->attrFamilly>=0&&j<curattr.size());
            }
        }
        //        glEnd();
    }
    
    
    
    
    
    posPingPong.dst->begin();
    updatePos.begin();  // Previus position
                        //    updatePos.setUniformTexture("tex0", posPingPong.dst->getTextureReference(), 0);      // Velocity
    updatePos.setUniformTexture("velData", velPingPong.src->getTextureReference(), 1);      // Velocity
    float a = timeStep;
    float b =timeStepM;
    float finalTimeStep =timeStep*timeStepM*1.0f/FPS ;
    updatePos.setUniform1f("timestep",finalTimeStep);
    updatePos.setUniform1i("resolution",textureRes);
    updatePos.setUniform1i("is2D",is2D?1:0);
    posPingPong.src->draw(0,0);
    
    
    
    updatePos.end();
    posPingPong.dst->end();

    
    posPingPong.swap();
    
    lastw = w;
    lasth = h;
    
}

void Particles::draw(int w, int h){


    
    ofEnableAlphaBlending();
    updateRender.begin();

    
    updateRender.setUniformTexture("posTex", posPingPong.src->getTextureReference(), 0);
    if(gradtype==2) updateRender.setUniformTexture("velTex",velPingPong.src->getTextureReference(),1);
    if(gradtype>0) updateRender.setUniformTexture("gradient",gradient.getTextureReference(),2);
#ifdef PIMG
    updateRender.setUniformTexture("img",partImg.getTextureReference(),3);
    updateRender.setUniform2f("imgRes",partImg.getWidth(),partImg.getHeight());

#endif
#ifdef CAMCOLO
    updateRender.setUniformTexture("camin", backImage.getTextureReference(), 4);
    updateRender.setUniform1i("iscam",iscam);
    updateRender.setUniform2f("insize",backImage.getWidth(),backImage.getHeight());
#endif

    updateRender.setUniform1i("resolution", (int)textureRes);
    updateRender.setUniform3f("screen", w, (float)h,(float)dad->zdepth);

    updateRender.setUniform4f("colorpart",gradtype,color.get()[0],color.get()[1],color.get()[2]);
    updateRender.setUniform2f("gradbounds",(float)mingrad,(float)maxgrad);
    //    updateRender.setUniform3f("mouse",(float)(ofGetMouse().x),(float)(height-mouseY),(float)zdepth/2 + f.mousez);
    updateRender.setUniform1f("alpha",alphaColor*alphaColorM/255.0);
    
    
    

    
    glPointSize(particleSize*w/1000.0);
    //    glBegin( GL_POINTS );
    //    for(int x = 0; x < textureRes; x++){
    //        for(int y = 0; y < textureRes; y++){
    //
    //            glVertex3d(x,y,0);
    //
    //        }
    //    }
    //    glEnd();
    partVbo.draw(GL_POINTS,0,numParticles);
    


    
    
    updateRender.end();

//    originLR.draw(0,0,scrw/2,scrh);
//    posPingPong.src->draw(0,0,scrw/2,scrh);
//    velPingPong.src->draw(scrw/2,0,scrw/2,scrh);

}


void Particles::initFbo(){
    initFbo(*dad->scrw,*dad->scrh);
}

void Particles::resetOrigins(bool & r){
    if(reset){
        reset=false;
        int oriType = lastOriType;
        origintype = -1;
        origintype = oriType;
        initFbo();
    }
}

void Particles::updateDeltas(){
    originLR.allocate(textureRes, textureRes,GL_RGB32F);
    if(numParticles*3!=originPos.size()){
        int a = numParticles;
        int b = textureRes;
        a*=1;
        b*=1;
        
    };
    float * deltas = new float[numParticles*3];
    float scale = 1;
    for (int x = 0; x < textureRes; x++){
        for (int y = 0; y < textureRes; y++){
            int i = textureRes * y + x;
            int i2 = textureRes * y + x+1;
            if(x==textureRes-1){
                i2 = textureRes * y ;
            }
            i2*=3;
            i*=3;



            deltas[i + 0] = (originPos[i2+0] - originPos[i + 0])*scale;
            deltas[i + 1] = (originPos[i2+1] - originPos[i + 1])*scale;
            deltas[i + 2] = (originPos[i2+2] - originPos[i + 2])*scale;

        }
    }
    originLR.getTextureReference().loadData(deltas, textureRes, textureRes, GL_RGB);
    originHB.allocate(textureRes, textureRes,GL_RGB32F);
    for (int x = 0; x < textureRes; x++){
        for (int y = 0; y < textureRes; y++){
            int i = textureRes * y + x;
            int i2 = textureRes * (y+1) + x;
            if(y==textureRes-1){
                i2 = x;
            }
            i*=3;
            i2*=3;
            deltas[i + 0] = originPos[i2+0] - originPos[i + 0];
            deltas[i + 1] = originPos[i2+1] - originPos[i + 1];
            deltas[i + 2] = originPos[i2+2] - originPos[i + 2];

        }
    }
    originHB.getTextureReference().loadData(deltas, textureRes, textureRes, GL_RGB);

    delete [] deltas;
}
void Particles::initFbo(int w,int h){
    
    // Seting the textures where the information ( position and velocity ) will be
    //    textureResx*textureResy = numParticles;
    //    textureResx/textureResy = scrw/scrh;

    textureRes2.x = (int)sqrt((float)numParticles* w/ h);
    textureRes2.y = (int)numParticles/textureRes2.x;
    
    textureRes = MAX(1,(int)sqrt((float)numParticles));
    numParticles = textureRes* textureRes;
    
    // Load this information in to the FBO´s texture
    if(!noReset){
        posPingPong.allocate(textureRes, textureRes,GL_RGB32F);


        // Load this information in to the FBO´s texture
        velPingPong.allocate(textureRes, textureRes,GL_RGB32F);
        origins.allocate(textureRes, textureRes,GL_RGB32F);


        if(originPos.size() < 3*numParticles  ){
            reset = true; // triggers resetOrigins
        }
        if(originPos.size()){
        posPingPong.src->getTextureReference().loadData(&originPos[0], textureRes, textureRes, GL_RGB);
        posPingPong.dst->getTextureReference().loadData(&originPos[0], textureRes, textureRes, GL_RGB);
        origins.getTextureReference().loadData(&originPos[0], textureRes, textureRes, GL_RGB);

        updateDeltas();
        }





        // 2. Making arrays of float pixels with velocity information and the load it to a texture
        float * vel = new float[numParticles*3];
        for (int i = 0; i < numParticles; i++){
            vel[i*3 + 0] = 0.0;
            vel[i*3 + 1] = 0.0;
            vel[i*3 + 2] = 0.0;//i*1.0/numParticles ;
        }

        velPingPong.src->getTextureReference().loadData(vel, textureRes, textureRes, GL_RGB);
        velPingPong.dst->getTextureReference().loadData(vel, textureRes, textureRes, GL_RGB);
        delete [] vel; // Delete the array


    }
    else{
        
        posPingPong.src->allocate(textureRes, textureRes,GL_RGB32F);
        posPingPong.src->begin();
        posPingPong.dst->draw(0,0);
        posPingPong.src->end();
        posPingPong.dst->allocate(textureRes,textureRes,GL_RGB32F);
        velPingPong.allocate(textureRes, textureRes,GL_RGB32F);
        origins.allocate(textureRes, textureRes,GL_RGB32F);
        updateDeltas();

    }
    partVbo.clear();
    partVbo.disableColors();
    partVbo.disableNormals();
    partVbo.disableTexCoords();
    partVbo.disableIndices();
    vector<ofVec3f> initPos (numParticles);
    for(int x = 0 ; x < textureRes ; x++){
        for(int y = 0 ; y < textureRes ; y++){
            initPos[x*textureRes+y] = ofVec3f(x,y,0);
        }
    }
    partVbo.setVertexData(&initPos[0],numParticles,GL_STATIC_DRAW);


}



void Particles::registerParam(){
    for(int i = 0; i<forces.size();i++){
        //        vector<ofAbstractParameter * > pl = forces[i].pl;
        forces[i]->settings.clear();
        settings.add(forces[i]->settings);
        
        for(int j = 0 ;j<forces[i]->pl.size();j++){

            
            forces[i]->settings.add(*forces[i]->pl[j]);

        }

    }
}

void Particles::changeNum(int & num){
    if(num>0 && num!=lastnumParticles){
        initFbo();
        lastnumParticles = num;
    }
}


void Particles::changeGrad(int & i){
    ofDirectory gradDir("gradients");
    if(i>=0 && i<gradDir.listDir()){
        gradDir.sort();
        gradDir.getFile(i).getFileName();
        gradient.loadImage("gradients/"+gradDir.getFile(i).getFileName());}
}




void Particles::changeOrigins(int &type){
    
    if(type==lastOriType){return;}
    lastOriType = type;
    bool hasChanged = true;
    
    noReset = true;
    
    switch (type) {
        case 0:
        {
            originPos.resize(numParticles*3);
            for (int x = 0; x < textureRes; x++){
                for (int y = 0; y < textureRes; y++){
                    int i = textureRes * y + x;
                    
                    originPos[i*3 + 0] = x*1.0/textureRes;
                    originPos[i*3 + 1] = y*1.0/(textureRes);
                    originPos[i*3 + 2] = 0.5;
                }
            }
            break;}
        case 1:
        {
            int zsplit = 2;
            
            textureRes3 =  MAX(1,(int)(pow(numParticles,1.0/3)+0.5));

            int curnumpart= textureRes3*textureRes3*textureRes3;
            ofLog() << "texture res "<<textureRes3 << "::" << curnumpart;
            textureRes = sqrt((float)curnumpart);

            int count=0;
            originPos.resize(curnumpart*3);
            for (int z = 0; z < textureRes3; z++){
                
                for (int x = 0; x < textureRes3; x++){
                    for (int y = 0; y < textureRes3; y++){


                        int i = textureRes3 * textureRes3* z + textureRes3 * y + x;
                        i*=3;
                        originPos[i + 0] = (float) (x*1.0/textureRes3);
                        originPos[i + 1] = (float) (y*1.0/textureRes3);
                        originPos[i + 2] = (float) (z*1.0/(textureRes3));
                        
                    }
                }
            }
            numParticles = curnumpart;
            //            ofSort(pos,sortOnXYZ);
            break;}
#ifdef PMOD
        case 2:
        {
//
//            pos = new float[numParticles*3];
//            for (int x = 0; x < textureRes; x++){
//                for (int y = 0; y < textureRes; y++){
//                    int i = textureRes * y + x;
//                    ofPoint p(x*1.0/textureRes,y*1.0/(textureRes),0.5);
//                    dad->sH->mapN2S(p,1);
//                    pos[i*3 + 0] = 0;//p.x;
//                    pos[i*3 + 1] = 0;//p.y;
//                    pos[i*3 + 2] = 0;//0.5;
//                }
//            }
//            break;
                        vector<ofPoint> vert = readObj("models/sphere159.obj",true);
                        int curNumPart = vert.size();
                        textureRes = ceil(sqrtf((float)numParticles));
                        curNumPart = textureRes*textureRes;
                        originPos.resize(curNumPart*3);
                        for (int x = 0; x < textureRes; x++){
                            for (int y = 0; y < textureRes; y++){
                                int i =  x + textureRes * y;

                                if(i<vert.size()){
                                originPos[i*3 + 0] = vert[i].x;
                                originPos[i*3 + 1] = vert[i].y;
                                originPos[i*3 + 2] = vert[i].z +1;
                                }
                                else {
                                    originPos[i*3 + 0] = 0;
                                    originPos[i*3 + 1] = 0;
                                    originPos[i*3 + 2] = 0;
                                }
                            }
                        }
                numParticles = curNumPart;
                        break;
        }

#endif
            
        default:
            hasChanged=false;
            break;
    }
    if(hasChanged){
        origins.allocate(textureRes, textureRes,GL_RGB32F);
        origins.getTextureReference().loadData(&originPos[0], textureRes, textureRes, GL_RGB);
        updateDeltas();
//        delete [] pos;
    }
    noReset = false;


}


vector<ofPoint> Particles::readObj(string pathin,bool sort){
    vector<ofPoint> points;
    string path = ofToDataPath(pathin, true);
    string line;



    // obj file format vertexes are 1-indexed
    points.push_back(ofPoint());

    ifstream myfile (path.c_str());
    if (myfile.is_open()) {
        while (! myfile.eof()) {
            getline (myfile,line);


            // parse the obj format here.
            //
            // the only things we're interested in is
            // lines beginning with 'g' - this says start of new object
            // lines beginning with 'v ' - coordinate of a vertex
            // lines beginning with 'f ' - specifies a face of a shape
            // 			we take each number before the slash as the index
            // 			of the vertex to join up to create a face.

            if(line.find("g ")==0) { // new object definition

            } else if(line.find("v ")==0) { // new vertex
                ofPoint p;
                vector<string> elements = ofSplitString(line, " ");
                if(elements.size()!=4) {
                    cout<<elements[0]+elements[1]+elements[2]+ofToString(elements.size())<<endl;
                    printf("Error line does not have 3 coordinates: \"%s\"\n", line.c_str());

                }
                
                p.x = atof(elements[1].c_str());
                p.y = atof(elements[2].c_str());
                p.z = atof(elements[3].c_str());
                points.push_back(p);
                
                
            } else if(line.find("f ")==0) { // face definition

            }
        }
    }


    myfile.close();

    
    if(sort){
        ofSort(points,sortOnXYZ);
    }
    
    return points;


}




