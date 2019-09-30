#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable
    
uniform sampler2DRect tex0;

void main()
{

    vec4 col=texture2DRect(tex0, gl_TexCoord[0].st);
    gl_FragColor = vec4(1.0,1.0,1.0,1.0) - col.rgba;


}