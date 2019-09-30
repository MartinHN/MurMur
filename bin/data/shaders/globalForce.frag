#pragma include "common.frag"





uniform sampler2DRect tex0;   // recive the previus velocity texture
uniform sampler2DRect posData;      // recive the position texture

uniform int resolution;
uniform vec2  screen;



// forces
uniform float f;
uniform float fmin;
uniform float vmax;
uniform float vmin;
uniform float comp;



    
void main(void){
    
    vec3 vel = texture2DRect(tex0, gl_TexCoord[0].xy).xyz;
    
    
    vel*=f;
    float velnorm = length(vel);
    if(velnorm>vmax){
        vel = normalize(vel)*(vmax+comp*(velnorm-vmax));
    }
    else if (velnorm>vmin){
        vel*=f;
    }
    else {
        vel*=fmin;
    }


    
    gl_FragColor.xyz = vel;

}
