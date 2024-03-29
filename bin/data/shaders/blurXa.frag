#pragma include "common.frag"



uniform sampler2DRect tex0;
uniform float blurAmnt;

void main()
{
    gl_FragColor.z =   .06 * texture2DRect(tex0, gl_TexCoord[0].st + vec2( -blurAmnt * 1.0, 0.0)).z
                    +.09 * texture2DRect(tex0, gl_TexCoord[0].st + vec2( -blurAmnt * .75, 0.0)).z
                    +.12 * texture2DRect(tex0, gl_TexCoord[0].st + vec2( -blurAmnt * .50, 0.0)).z
                    +.15 * texture2DRect(tex0, gl_TexCoord[0].st + vec2( -blurAmnt * .25, 0.0)).z
                    
                    +.16 * texture2DRect(tex0, gl_TexCoord[0].st + vec2(blurAmnt, 0.0)).z
                    
                    +.15 * texture2DRect(tex0, gl_TexCoord[0].st + vec2( blurAmnt * .25, 0.0)).z
                    +.12 * texture2DRect(tex0, gl_TexCoord[0].st + vec2( blurAmnt * .50, 0.0)).z
                    +.09 * texture2DRect(tex0, gl_TexCoord[0].st + vec2( blurAmnt * .75, 0.0)).z
                    +.06 * texture2DRect(tex0, gl_TexCoord[0].st + vec2( blurAmnt * 1.0, 0.0)).z;
}
