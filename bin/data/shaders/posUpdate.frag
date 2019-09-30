#pragma include "common.frag"


uniform sampler2DRect tex0;  // recive the previus position texture
uniform sampler2DRect velData;      // recive the velocity texture


uniform float timestep;
uniform int resolution;


uniform int is2D;


void main(void){
    
    
    
    vec2 st = gl_TexCoord[0].st ;
    
    
    vec3 pos = texture2DRect(tex0,st).xyz; // ... in order to look at a particulary place on it
    vec3 vel = texture2DRect( velData, st ).xyz;     // Fetch both the pos and vel.
    //vel.x*=1.7;
    vec3 addVel = vel*timestep;
    if(is2D>0){
        pos += vec3(addVel.xy,0) ; // Updates the position
    }
    else{
        pos+=addVel.xyz;
    }
//    }
    //if(borders.x==1){
//        if ( pos.x < 0.0)
//        {pos.x=0;}
//        
//        else if ( pos.x > 1.0)
//        {pos.x=1.0;}
//        
//        if (pos.y < 0.0)
//        {pos.y=0;}
//        
//        else if ( pos.y > 1.0)
//        {pos.y=1.0;}
//    
//    if (pos.z < 0.0)
//    {pos.z=0;}
//    
//    else if ( pos.z > 1.0)
//    {pos.z=1.0;}
    
    //}
    gl_FragColor.xyz = pos;  // And finaly it store it on the position FBO
//}
}
