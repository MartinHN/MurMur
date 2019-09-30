#pragma include "common.frag"


uniform sampler2DRect tex0;


uniform float thresh;
uniform float colOn;
uniform float colOff;
void main()
{

    vec4 col=texture2DRect(tex0, gl_TexCoord[0].st);
//    float intensity = dot(col, vec4(0.299, 0.587, 0.114,0));

  
    
    if(col.w>0 && col.r>thresh)gl_FragColor = vec4(colOn,colOn,colOn,1);
    else gl_FragColor = vec4(colOff,colOff,colOff,1);
    

}
