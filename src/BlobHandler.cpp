//
//  BlobHandler.cpp
//  MurMur d'Ombre
//
//  Created by martin hermant on 17/02/14.
//
//



#include "BlobHandler.h"
#if USE_SYPHON
#include "ofxSyphon.h"
#elif USE_KINECT
#include "ofxKinect.h"
#elif USE_REMOTE_KINECT
#include "ofxOscReceiver.h"
#else
#pragma error should  use at least osc kinect
#endif

#define USE_ONE_CHANNEL 1
BlobHandler::BlobHandler(){
#if USE_SYPHON
    blobClient.reset( new ofxSyphonClient());
#elif USE_KINECT
    kinect.reset(new ofxKinect());
#elif USE_REMOTE_KINECT
    oscRcv.reset(new ofxOscReceiver());
#endif

}
BlobHandler::~BlobHandler(){}
void BlobHandler::setup(int inwin, int inhin,ScreenHandler * sHin){
    inw = inwin;
    inh = inhin;
    sH = sHin;
#if USE_ONE_CHANNEL
    threshBW.load("","shaders/thresholdBW.frag");
#endif

}

void BlobHandler::setupData(ofShader* blurXin,ofShader * blurYin){
    blurX=blurXin;
    blurY = blurYin;
#if USE_SYPHON
    blobClient->setup();
    blobClient->setApplicationName("kinectExample");
    //    blobClient.setApplicationName("kinectExampleDebug");
    //    blobClient.setServerName("blob");
    //    blobClient.setApplicationName("Simple Server");
    //    blobClient.setApplicationName("Arena");
    //    blobClient.setServerName("");
#elif USE_KINECT
    kinect->init(false,false);
    //kinect.init(true); // shows infrared instead of RGB video image
    //kinect.init(false, false); // disable video image (faster fps)

    kinect->open();        // opens first available kinect
                           // print the intrinsic IR sensor values
    if(!kinect->isConnected()) {
        ofLogError() << "can't connect kinect";
    }
#elif USE_REMOTE_KINECT

    oscRcv->setup(8888);
#endif


#if USE_ONE_CHANNEL
    depthTex.allocate(inw,inh,GL_R8);
    //    pix.allocate(inw,inh,1);
#else
    depthTex.allocate(inw,inh,GL_RGB);
    pix.allocate(inw,inh,3);
#endif


    gs.allocate(inw, inh);
    lastw = inw;
    lasth = inh;

    colorIm.allocate(inw,inh);
}


void BlobHandler::update(){
#if USE_REMOTE_KINECT
    ofxOscMessage msg;
    bool hadMessage = false;
    while(oscRcv->getNextMessage(msg)){hadMessage=true;}
    if(hadMessage){
        int idx = 0;
        int numBlobs = msg.getArgAsInt(0); // getNumBlobs
        idx++;
        blobs.resize(numBlobs);
        for(int i = 0 ; i < numBlobs ; i++){ // get independent blobSizes
            int blobSize = msg.getArgAsInt(idx);
            blobs[i].pts.resize(blobSize);
            blobs[i].nPts = blobSize;
            idx++;
        }
        for(int i = 0 ; i < numBlobs ; i++){ // fill blobs
            for(int j=0 ; j< blobs[i].nPts; j++){
                blobs[i].pts[j].x=msg.getArgAsFloat(j+idx);
                idx++;
                blobs[i].pts[j].y=msg.getArgAsFloat(j+idx);
                idx++;
                blobs[i].pts[j].z=0;
            }
        }


    }
#endif
    if(computeBlob){
        updateDepthTex();
        getGS();

        compBlob();
        compCache();
        arms = compExtrems();
        centroids = compCentroid();
        boxes = compBounds();
    }
    else{
        arms.clear();
        centroids.clear();
        boxes.clear();
    }


    //    blurblob();

}

template <class T>
ofReadOnlyParameter<T,float> makeRO(ofParameter<T> & p){
    return ofReadOnlyParameter<T,float>(p);
}
#define SETPERSISTENT(x) {auto ro =makeRO(x); x.setSerializable(false);persistentGroup.add(ro);}

void BlobHandler::registerParams(){
    settings.setName("blobsettings");

    MYPARAM(isPiping,false,false,true);
    MYPARAM(computeBlob,true,false,true);
    MYPARAM(vidThreshold, 70.f, 0.f, 255.f);
    MYPARAM(blobBlur, 100.f, 0.f, 255.f);
    MYPARAM(invertBW,true,false,true);
    
    MYPARAM(minSide,0.f,0.f,1.f);
    MYPARAM(maxSide,1.f,0.f,1.f);
    MYPARAM(maxBlobs, 1,0,4);
    MYPARAM(findHoles,false,false,true);
    MYPARAM(simplification,0.f,0.f,.1f);
    MYPARAM(smooth,0.f,0.f,30.f);
    MYPARAM(polyMaxPoints, 0,0,200);
    MYPARAM(maxLengthExtrem, 15.f,0.f,100.f);
    MYPARAM(maxArmWidth, .05f,0.01f,.2f);
    MYPARAM(screenN, 0, 0, 123);
    MYPARAM(invertX,false,false,true);
    MYPARAM(invertY,false,false,true);
    MYPARAM(pos, ofVec3f(0.5),ofVec3f(0),ofVec4f(1.));
    MYPARAM(scale, ofVec3f(1),ofVec3f(0),ofVec4f(2.));
    MYPARAM(crop, ofVec4f(0),ofVec4f(0),ofVec4f(100));
    
    
    SETPERSISTENT(vidThreshold);

    
}



//
//
//void BlobHandler::computePoly(){
//
//    depthTex.dst->begin();
//    threshBW.begin();
//    threshBW.setUniformTexture("tex",blobClient.getTexture(),1);
//
//
//    threshBW.end();
//    depthTex.dst->end();
//
//    ofxCvGrayscaleImage bw ;
//    bw.getTextureReference() = depthTex.src->getTextureReference();
//    bw.threshold(40);
//
//}

void BlobHandler::updateDepthTex(){
#if USE_SYPHON
#if USE_ONE_CHANNEL
    //    glBlendEquation(GL_FUNC_ADD_EXT);
    //
    //    glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_DST_ALPHA);
    //
    //    depthTex.dst->begin();
    //    ofSetColor(255);
    //    blobClient.draw(0,0);
    //    depthTex.dst->end();
    //    depthTex.swap();

#else
    glBlendEquation(GL_FUNC_ADD_EXT);

    glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_DST_ALPHA);

    depthTex.begin();
    ofSetColor(255);
    blobClient->draw(0,0);
    depthTex.end();
#endif
#elif USE_KINECT
    kinect->update();
    //    if(kinect.isFrameNewDepth()) { // there is a new frame and we are connected
    //        calib.computeOnTexture(kinect.getDepthTextureReference(),true);

#elif USE_REMOTE_KINECT
#endif


}

void BlobHandler::getGS(){
#if USE_ONE_CHANNEL
    glBlendEquation(GL_FUNC_ADD_EXT);


    glBlendColor(1,1,1,blobBlur/255.0);
    glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
    //    glBlendFunc(GL_CONSTANT_ALPHA, GL_CONSTANT_ALPHA);
    depthTex.begin();
    if(vidThreshold>0 ){
        threshBW.begin();
        threshBW.setUniform1f("thresh",(float)(vidThreshold/255.0f));
        threshBW.setUniform1f("colOn",invertBW?0:1);
        threshBW.setUniform1f("colOff",invertBW?1:0);
    }
    //    ofSetColor(0);//,0,0,blobBlur);
    //    ofRect(0, 0, blobClient.getWidth(), blobClient.getHeight());

    //    ofSetColor(255);
#if USE_SYPHON
    blobClient->draw(0,0);
#elif USE_KINECT
    kinect->getDepthTexture().draw(0,0);
#elif USE_REMOTE_KINECT
#pragma error TODO
#endif
    if(vidThreshold>0){
        threshBW.end();
    }
    depthTex.end();
    ofEnableAlphaBlending();
    auto & tr = depthTex.getTextureReference();
    tr.bind();
    glGetTexImage(tr.texData.textureTarget,0,GL_RED,GL_UNSIGNED_BYTE, gs.getPixels().getData());
    tr.unbind();
    
    //    depthTex.dst->readToPixels(pix);
    //    gs.setFromPixels(pix);
    //    gs.updateTexture();

#else

    getDepthTex().readToPixels(pix);
    colorIm.setFromPixels(pix);
    colorIm.updateTexture();
    gs = colorIm;
#endif
    ///////// old


}



const ofTexture & BlobHandler::getDepthTex()const{
#if USE_SYPHON
    return depthTex;
#elif USE_KINECT
    return kinect->getDepthTexture();
#endif
}
void BlobHandler::compBlob(){
    //#if !USE_ONE_CHANNEL
    if(vidThreshold >0){
        gs.threshold(vidThreshold,invertBW);
    }
    //#endif
    contourFinder.findContours(gs, minSide*inw*inh*minSide, maxSide*inw*inh*maxSide, maxBlobs, findHoles);

    blobs = contourFinder.blobs;

    lastcw = 1;
    lastch = 1;


}


vector<ofVec3f> BlobHandler::compCentroid(float w, float h){
    vector<ofVec3f> res;
    ofVec3f scalel(w,h);
    for (int i = 0 ; i< blobs.size();i++){

        ofVec2f realC = cachedP[i].getCentroid2D();


        res.push_back(realC*scalel);
    }
    return res;
}

vector<ofRectangle> BlobHandler::compBounds(float w, float h){
    vector<ofRectangle> res;

    for (int i = 0 ; i< blobs.size();i++){
        ofRectangle rr = cachedP[i].getBoundingBox();

        res.push_back(ofRectangle(rr.x*w,rr.y*h,rr.width*w,rr.height*h));
    }
    return res;
}

void BlobHandler::compCache(){
    cachedP.clear();
    ofRectangle rr = sH->rectOfScreen(sH->getValidScreen(screenN));
    ofVec2f coef (1.0/(*sH->scrw), 1.0/(*sH->scrh));
    rr.x*=coef.x;
    rr.y*=coef.y;
    rr.width*=coef.x;
    rr.height*=coef.y;


    for (int i = 0 ; i< blobs.size();i++){
        ofPolyline pp ;

        for(int j = 0 ; j < blobs[i].nPts;j++){
            ofVec3f p = blobs[i].pts[j]/ofVec3f(inw,inh,1);
            if(invertX)p.x = 1-p.x;
            if(invertY)p.y = 1-p.y;
            p  =  ofVec2f(0.5) + (p-ofVec2f(0.5)) * scale.get();
            p +=  (pos.get()-ofVec2f(0.5));



            p.x = ofMap(p.x, 0, 1, rr.getMinX() ,rr.getMaxX());
            p.y =ofMap(p.y, 0, 1, rr.getMinY() ,rr.getMaxY());

            pp.lineTo(p);
        }


        if(simplification>0){
            pp.simplify(simplification);
        }
        if(smooth>0){
            pp = pp.getSmoothed(smooth);
        }
        if(polyMaxPoints>0){pp=pp.getResampledByCount(polyMaxPoints);}
        //        if(polySpacing>0){pp=pp.getResampledBySpacing(polySpacing);}
        if(pp.size()>0){
            pp.close();

            cachedP.push_back(pp);
        }

    }


}

vector<ofPolyline> BlobHandler::getBlobs(float w, float h,bool invx,bool invy){
    vector<ofPolyline> res;
    if(w==1 &&  h==1){
        res = cachedP;
    }
    else if(w==lastcw && h == lastch){
        res = lcacheP;
    }
    else{

        ofVec2f ts = ofVec2f(w ,h);
        polyCacheDirty = false;
        for(int i = 0 ; i< cachedP.size() ; i++){
            ofPolyline l;
            auto pl = cachedP[i].getVertices();
            for (int  j = 0 ; j< pl.size() ; j++){
                ofPoint lp;
                lp.x = !invx? pl[j].x : 1-pl[j].x;
                lp.y = !invy ? pl[j].y :  1-pl[j].y;
                lp *= ts;
                l.lineTo(lp);
            }
            res.push_back(l);

        }
        lcacheP = res;
        lastcw = w;
        lastch = h;

    }
    return res;


}

vector<ofPath> BlobHandler::getPaths(float w, float h,bool invx,bool invy){
    vector<ofPath> res;
    vector<ofPolyline> p;

    p = getBlobs(w,h,invx,invy);

    for (int i = 0 ; i< p.size();i++){
        ofPath pp;

        for(int j = 0 ; j < p[i].size();j++){
            //            ofPoint tst = p[i][j];
            pp.lineTo(p[i][j]);
        }
        pp.close();
        if(p[i].size()>0)
            res.push_back(pp);
    }
    return res;
}

vector<ofVec3f> BlobHandler::compExtrems(float w, float h){
    vector<ofVec3f> res;
    //    vector<ofPolyline> tmp ;




    float sum_angles=0;
    int begin=0,end=1;
    float maxangle = 170;


    for (int i = 0 ; i< cachedP.size();i++){
        begin=0;
        end=1;
        sum_angles=0;
        const ofPolyline & tmpspaced = cachedP[i];//.getSmoothed(0.01);
        
        deque<float> tmpp;
        if(tmpspaced.size()<maxLengthExtrem){continue;}

        while(end<tmpspaced.size()+maxLengthExtrem){

            auto cur_angle =tmpspaced.getAngleAtIndex(end%tmpspaced.size())* (tmpspaced.getRotationAtIndex(end%tmpspaced.size()).z>0?1:-1)*(invertX?-1:1);

            if(end-begin<=maxLengthExtrem){

                tmpp.push_back(cur_angle );
                sum_angles+=tmpp.back();
                end++;
            }
            else if(tmpp.size()>0){
                sum_angles-=tmpp.front();
                tmpp.pop_front();

                tmpp.push_back(cur_angle);
                sum_angles+=tmpp.back();

                begin++;
                end++;
            }
            else{
                break;
            }
            
            if(sum_angles<-maxangle/4){
                sum_angles-=tmpp.front();
                tmpp.pop_front();
                begin++;
                if(end<= begin){
                    end = begin;
                    sum_angles=0;
                }
            }
            else if( sum_angles>maxangle && end-begin>maxLengthExtrem/2){

                if(glm::distance(tmpspaced[begin%tmpspaced.size()],tmpspaced[end%tmpspaced.size()])>maxArmWidth){
                    continue;
                }
                float idx = begin;
                float sum = 0;
                for (int j = 0 ; j < tmpp.size() ; j++){
                    sum+= tmpp[j]*(j+1);
                }
                sum/=sum_angles;
                sum-=1;
                idx+=sum;
                if(idx>tmpspaced.size()){
                    idx-=tmpspaced.size();
                }

                ofPoint p =tmpspaced.getPointAtIndexInterpolated(idx);
                ofPoint center (0);
                for(int i = begin ; i < end ; i++){
                    center+=tmpspaced[i%tmpspaced.size()];
                }
                center/=(end-begin);

                bool test = true;
                ofVec2f l = (p-center).getNormalized();
                l = l.getPerpendicular();


                for(int i = begin ; i < end ; i++){
                    auto  pp = tmpspaced[i%tmpspaced.size()] -center;

                    if(abs(glm::dot(glm::tvec2<float>(pp),glm::tvec2<float>(l)))>maxArmWidth){
                        test = false;
                    }
                }
                if(!test){continue;}
                //                if(abs((p-tmpspaced.getCentroid2D()).getNormalized().dot(ofVec2f(1,0)))>cos(ofDegToRad(80))){
                //                if(center.distance(p)>0.1){



                //                    for(int i = begin ; i < end ; i++){
                //                        auto & pp = tmpspaced[i%tmpspaced.size()] ;
                //                        res.push_back(pp*ofVec2f(w,h));
                //                    }
                //                    res.push_back(center*ofVec2f(w,h));
                res.push_back(p*ofVec2f(w,h));

                //                    if(test == false)ofLogWarning("no extrem");
                begin = end;
                sum_angles=0;
                tmpp.clear();
            }
            //            }
        }
    }


    return res;
}
//
//void BlobHandler::blurblob(){
//    ofPushMatrix();
//    ofPushStyle();
//    ofPushView();
//
//    glBlendEquation(GL_FUNC_ADD_EXT);
//
//    glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_DST_ALPHA);
//    gs.dilate_3x3();
//
//    depthTex.dst->begin();
//    ofSetColor(255);
//    blurX->begin();
//    blurX->setUniform1f("blurAmnt", blobBlur);
//    gs.draw(0,0,inw,inh);
//    blurX->end();
//    depthTex.dst->end();
//    glFlush();
//
//    depthTex.swap();
//
//    depthTex.dst->begin();
//    blurY->begin();
//    blurY->setUniform1f("blurAmnt", blobBlur);
//    depthTex.src->draw(0,0,inw,inh);
//    blurY->end();
//    depthTex.dst->end();
//    depthTex.swap();
//    ofPopMatrix();
//    ofPopStyle();
//    ofPopView();
//}
//
//
