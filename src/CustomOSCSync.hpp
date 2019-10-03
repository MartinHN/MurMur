//
//  CustomOSCSync.hpp
//  of_interface
//
//  Created by Martin Hermant on 01/10/2019.
//
#pragma once

class CustomSender : public ofxOscSender{
public:
    void appendParameter(ofxOscMessage &msg, const ofAbstractParameter &parameter, const std::string &address){
        msg.setAddress(address+parameter.getEscapedName());
        if(parameter.type() == typeid(ofParameter<int>).name()){
            msg.addIntArg(parameter.cast<int>());
        }
        else if(parameter.type() == typeid(ofParameter<float>).name()){
            msg.addFloatArg(parameter.cast<float>());
        }
        else if(parameter.type() == typeid(ofParameter<double>).name()){
            msg.addDoubleArg(parameter.cast<double>());
        }
        else if(parameter.type() == typeid(ofParameter<bool>).name()){
            msg.addBoolArg(parameter.cast<bool>());
        }
        else{
            msg.addStringArg(parameter.toString());
        }
    }
};

class CustomRcvr : public ofxOscReceiver{
public:
    bool getParameter(ofAbstractParameter &parameter){
        ofxOscMessage msg;
        while(getNextMessage(msg)){
            ofAbstractParameter * p = &parameter;
            std::vector<std::string> address = ofSplitString(msg.getAddress(),"/", true);
            for(unsigned int i = 0; i < address.size(); i++){
                if(p){
                    if(address[i] == p->getEscapedName()){
                        if(p->type() == typeid(ofParameterGroup).name()){
                            if(address.size() >= i+1){
                                ofParameterGroup* g = static_cast<ofParameterGroup*>(p);
                                if(g->contains(address[i+1])){
                                    p = &g->get(address[i+1]);
                                }
                                else{
                                    p = nullptr;
                                }
                            }
                        }
                        else if(p->type() == typeid(ofParameter<int>).name() &&
                                msg.getArgType(0) == OFXOSC_TYPE_INT32){
                            p->cast<int>() = msg.getArgAsInt32(0);
                        }
                        else if(p->type() == typeid(ofParameter<float>).name() &&
                                msg.getArgType(0) == OFXOSC_TYPE_FLOAT){
                            p->cast<float>() = msg.getArgAsFloat(0);
                        }
                        else if(p->type() == typeid(ofParameter<double>).name() &&
                                msg.getArgType(0) == OFXOSC_TYPE_DOUBLE){
                            p->cast<double>() = msg.getArgAsDouble(0);
                        }
                        else if(p->type() == typeid(ofParameter<bool>).name() &&
                                (msg.getArgType(0) == OFXOSC_TYPE_TRUE ||
                                 msg.getArgType(0) == OFXOSC_TYPE_FALSE ||
                                 msg.getArgType(0) == OFXOSC_TYPE_INT32 ||
                                 msg.getArgType(0) == OFXOSC_TYPE_INT64 ||
                                 msg.getArgType(0) == OFXOSC_TYPE_FLOAT ||
                                 msg.getArgType(0) == OFXOSC_TYPE_DOUBLE ||
                                 msg.getArgType(0) == OFXOSC_TYPE_STRING ||
                                 msg.getArgType(0) == OFXOSC_TYPE_SYMBOL)){
                                    p->cast<bool>() = msg.getArgAsBool(0);
                                }
                        else if(p->type()==typeid(ofParameter<ofVec4f>).name() && msg.getArgType(0)==OFXOSC_TYPE_FLOAT ){
                            if(msg.getNumArgs()!=4){ofLogWarning("recieving wrong vector size : "+p->getName()+" with "+ ofToString(msg.getNumArgs())+" values");}
                            else{
                                //currentParameterSet = p;
                                ofVec4f vt;
                                for(int i  = 0 ; i < 4 ; i++){
                                    vt[i] = msg.getArgAsFloat(i);
                                }
                                (p->cast<ofVec4f>())=vt;
                            }
                        }
                        else if(p->type()==typeid(ofParameter<ofVec3f>).name() && msg.getArgType(0)==OFXOSC_TYPE_FLOAT ){
                            if(!(msg.getNumArgs()==3 || msg.getNumArgs()==2)){ofLogWarning("recieving wrong vector size : "+p->getName()+" with "+ ofToString(msg.getNumArgs())+" values");}
                            else{
                                //currentParameterSet = p;
                                ofVec3f vt;
                                vt.z = p->cast<ofVec3f>().get().z;
                                for(int i  = 0 ; i < msg.getNumArgs(); i++){
                                    vt[i] = msg.getArgAsFloat(i);
                                }
                                (p->cast<ofVec3f>())=vt;

                            }

                        }

                        else if(p->type()==typeid(ofParameter<ofVec2f>).name() && msg.getArgType(0)==OFXOSC_TYPE_FLOAT){
                            if(msg.getNumArgs()!=2){ofLogWarning("recieving wrong vector size : "+p->getName()+" with "+ ofToString(msg.getNumArgs())+" values");}
                            else{
                                //currentParameterSet = p;
                                ofVec2f vt;
                                for(int i  = 0 ; i < 2 ; i++){
                                    vt[i] = msg.getArgAsFloat(i);
                                }
                                (p->cast<ofVec2f>())=vt;
                            }
                        }

                        else if(msg.getArgType(0) == OFXOSC_TYPE_STRING){
                            p->fromString(msg.getArgAsString(0));
                        }
                    }
                }
            }
        }
        return true;
    }
};

class CustomOSCSync{
    public:
    CustomOSCSync(){
        updatingParameter = false;
    }

    //--------------------------------------------------------------
    ~CustomOSCSync(){
        ofRemoveListener(syncGroup.parameterChangedE(), this, &CustomOSCSync::parameterChanged);
    }

    //--------------------------------------------------------------
    void setup(ofParameterGroup &group, int _localPort, const std::string &host, int _remotePort){
        localPort = _localPort;
        remotePort = _remotePort;
        syncGroup = group;
        ofAddListener(syncGroup.parameterChangedE(), this, &CustomOSCSync::parameterChanged);
        setRemoteIp(host);
    }
    void setRemoteIp(const std::string & host){
        sender.setup(host, remotePort);
        receiver.setup(localPort);
    }
    //--------------------------------------------------------------
    void update(){
        if(receiver.hasWaitingMessages()){
            updatingParameter = true;
            receiver.getParameter(syncGroup);
            updatingParameter = false;
        }
    }

    //--------------------------------------------------------------
    void parameterChanged(ofAbstractParameter &parameter){
        if(updatingParameter) return;
        sender.sendParameter(parameter);
    }

    CustomSender sender; ///< sync sender
    CustomRcvr receiver; ///< sync receiver
    ofParameterGroup syncGroup; ///< target parameter group
    bool updatingParameter; ///< is a parameter being updated?
    int localPort;
    int remotePort;
};
