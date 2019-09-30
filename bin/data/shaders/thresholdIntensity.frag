#pragma include "common.frag"

uniform sampler2DRect tex0;


uniform float thresh;
void main()
{

    vec4 col=texture2DRect(tex0, gl_TexCoord[0].st);
//    float intensity = dot(col, vec4(0.299, 0.587, 0.114,0));


    if(col.w>thresh)gl_FragColor = col;
    else gl_FragColor = vec4(0,0,0,0);
    

}
