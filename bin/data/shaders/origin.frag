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
uniform float applyOnBorder;



void main(void){
    
    vec3 vel = texture2DRect(tex0, gl_TexCoord[0].xy).xyz;
    
    vec3 ori =  texture2DRect( originData, gl_TexCoord[0].xy).xyz;
    vec3 pos = texture2DRect( posData, gl_TexCoord[0].xy).xyz;
    float maxPixBorder = resolution*applyOnBorder/2 + 0.5;
    if(applyOnBorder==0 || gl_TexCoord[0].x<=maxPixBorder || gl_TexCoord[0].y<=maxPixBorder || gl_TexCoord[0].x>=resolution-maxPixBorder-1 || gl_TexCoord[0].y>=resolution-maxPixBorder-1){
        float noiseF =0.0;
        if(noiseAmp>0.0){
            noiseF=noiseAmp*sin((noiseFreq*ori.y + noisePhase)*3.1415);
            //        noiseF= (noiseF + 1.0)/2.0;
            
        }
        ori.x+=noiseF;
        
        
        vec3 velbuf = (ori-pos)*k;
        if( velbuf.x !=0 ||  velbuf.y != 0 ){
            vel+=velbuf-normalize(velbuf)*dot(vel,normalize(velbuf))*z;
        }
        
        
        if(freeze>0&&length(ori-pos)<freeze &&length(ori-pos)>freezemin ){
            vel*=damp;
            
        }
        
        
        //vel.x+= 0.1;
        //	if(vel.x>1.0){
        //		 vel.x=0;
        //	}
        
        
    }
    
    gl_FragColor.xyz = vel;
    
}
