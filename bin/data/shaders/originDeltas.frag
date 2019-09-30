#pragma include "common.frag"



uniform sampler2DRect tex0;   // recive the previus velocity texture
uniform sampler2DRect posData;      // recive the position texture
uniform sampler2DRect originDeltasLR;      // recive the position texture
uniform sampler2DRect originDeltasHB;      // recive the position texture

uniform int resolution;
uniform vec2  screen;




// forces

uniform float k;
uniform float z;
uniform vec3  stretch;




void main(void){
    vec2 st=gl_TexCoord[0].xy;
    vec3 vel = texture2DRect(tex0, st).xyz;
    vec3 pos = texture2DRect( posData, st).xyz;


//    deltalr.x*=stretch.x;
//    deltalr.y*=stretch.y;
//    deltalr.z*=stretch.z;
//
//    deltahb.x*=stretch.x;
//    deltahb.y*=stretch.y;
//    deltahb.z*=stretch.z;

    vec3 distbuf;
    vec3 dVel = vec3(0) ;
    if(st.x<resolution-1){
        vec3 deltalr = texture2DRect( originDeltasLR, st).xyz;
        vec3 opos = texture2DRect( posData , st+vec2(1,0)).xyz;
        distbuf=opos-pos;
        dVel-= (deltalr*stretch- distbuf)*k;

    }
    if(st.y<resolution-1){
        vec3 deltahb = texture2DRect( originDeltasHB, st).xyz;
    vec3 opos = texture2DRect( posData , st+vec2(0,1)).xyz;
    distbuf=opos-pos;
    dVel-= (deltahb*stretch- distbuf)*k;
    }



    if(st.x>1){
        vec3 ldeltalr = -texture2DRect( originDeltasLR, st+vec2(-1,0)).xyz;
        vec3 opos = texture2DRect( posData , st+vec2(-1,0)).xyz;
        distbuf=opos-pos;
        dVel-= (ldeltalr*stretch- distbuf)*k;

    }
    if(st.y>1){
        vec3 ldeltahb = -texture2DRect( originDeltasHB, st+vec2(0,-1)).xyz;
        vec3 opos = texture2DRect( posData , st+vec2(0,-1)).xyz;
        distbuf=opos-pos;
        dVel-= (ldeltahb*stretch- distbuf)*k;
    }


    float lDvel = length(dVel);
    if(lDvel>0){
        vec3 ndVel = dVel/lDvel;
        dVel-=ndVel*dot(vel,ndVel)*z;
    }

//    float vLen = length(vel);
//    float dvLen = length(dVel);
//    float contrib = 1;
//    if(vLen>0 && dvLen>0){
//        contrib = dot(vel,-dVel)/(vLen*dvLen);
//        contrib = max(-vMax*100.0,contrib);
//    }



//    vel+=contrib*dVel;

    vel+=dVel;
    
    gl_FragColor.xyz = vel;
    
}
