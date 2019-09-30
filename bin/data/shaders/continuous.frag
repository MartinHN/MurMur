#pragma include "common.frag"



uniform sampler2DRect tex0;   // recive the previus velocity texture
uniform sampler2DRect posData;      // recive the position texture

uniform int resolution;
uniform vec2  screen;

uniform vec3 attr;

// forces

uniform vec3 dir;



    
void main(void){
    vec2 st=gl_TexCoord[0].xy;
    vec3 vel = texture2DRect(tex0, st).xyz;
    vec3 pos = texture2DRect( posData, st).xyz;   

    
    gl_FragColor.xyz = vel + dir;

}
