#pragma include "common.frag"


// This fill the billboard made on the Geometry Shader with a texture



uniform float alpha;




void main() {
    
    
    vec2 st = 2.0*gl_PointCoord.st-vec2(1.0,1.0);
    float curalpha = alpha*max(0.0,(1.0-length(st)));//max(0.f,1.f-texture2DRect(Tex,vec2(l,1)).x);
//    if(curalpha>0){
//        curalpha = alpha;
//    }
    gl_FragColor = vec4(gl_Color.xyz,curalpha*curalpha*(3.0 - 2.0 * curalpha));
//    gl_FragColor = vec4(gl_Color.xyz,curalpha);



}
