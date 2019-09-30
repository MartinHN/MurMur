#pragma include "common.frag"





uniform sampler2DRect tex0;   // recive the previus velocity texture
uniform sampler2DRect posData;      // recive the position texture
uniform sampler2DRect originData;      // recive the origin

uniform int resolution;
uniform vec2  screen;



// forces

uniform float k;
uniform float z;
uniform float freeze;
uniform float freezemin;
uniform float damp;
uniform float noiseFreq;
uniform float noiseAmp;
uniform float noisePhase;



void main(void){
    
    vec3 vel = texture2DRect(tex0, gl_TexCoord[0].xy).xyz;
    
    vec3 ori =  texture2DRect( originData, gl_TexCoord[0].xy).xyz;
    vec3 pos = texture2DRect( posData, gl_TexCoord[0].xy).xyz;
    float noiseF =0.0;
    if(noiseAmp>0.0){
        noiseF=noiseAmp*sin((noiseFreq*ori.y + noisePhase)*3.1415);
        //        noiseF= (noiseF + 1.0)/2.0;
        
    }
    ori.x+=noiseF;
    
    
    float velbuf = (ori.z+0.5-pos.z)*k;
    
    
    vel.z+=velbuf-vel.z*z;
    
    
    if(freeze>0&&length(ori-pos)<freeze &&length(ori-pos)>freezemin ){
        vel.z*=damp;
        
    }
    
    
    //vel.x+= 0.1;
    //    if(vel.x>1.0){
    //         vel.x=0;
    //    }
    
    
    
    
    gl_FragColor.xyz = vel;
    
}
