#pragma include "common.frag"





uniform sampler2DRect tex0;   // recive the previus velocity texture
uniform sampler2DRect posData;      // recive the position texture

uniform int resolution;
uniform vec2  screen;



// forces

uniform float k;
uniform float l0;
uniform float l0max;
uniform float z;
uniform int size;




void main(void){
    vec2 st=gl_TexCoord[0].xy;
    vec3 vel = texture2DRect(tex0, st).xyz;
    vec3 pos = texture2DRect( posData, st).xyz;
    float l0norm = l0/resolution;
    vec3 distbuf;
    float normbuf;
    vec3 linbuf;
    float leftNormBuf;
    float l0maxnorm;
    if(l0max==0){
        l0maxnorm = 9999;
    }
    else{
        l0maxnorm = l0max/resolution;
    }

    for(int i = 1 ; i<=size;i++){
        vec3 poso = texture2DRect( posData , st+vec2(0,i)).xyz;
        vec3 opos = texture2DRect( posData , st+vec2(0,-i)).xyz;

        leftNormBuf = -1.0;
        //        linbuf= vec3(l0max+1);

        if(st.y<resolution-i){
            distbuf=pos-poso;
            linbuf=distbuf;
            normbuf = length(distbuf);
            leftNormBuf = normbuf;
            if(normbuf>0 && normbuf<l0maxnorm){
                vel-=normalize(distbuf)*(normbuf-i*l0norm)*k;
            }
        }
        if(st.y>i){
            distbuf=pos-opos;
            normbuf = length(distbuf);

            if(normbuf>0 && normbuf<l0maxnorm){
                vel-=normalize(distbuf)*(normbuf-i*l0norm)*k;
                //            if(st.y<resolution-1&&length(distbuf)<netmax){
                //                vel-=z*(distbuf+linbuf);
                //            }
                if(leftNormBuf<l0maxnorm && leftNormBuf>=0){
                    vel-=z*(distbuf+linbuf);
                }
            }

            
            
            
        }
        
    }
    

    
    gl_FragColor.xyz = vel;
    
}
