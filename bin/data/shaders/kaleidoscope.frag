#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2DRect tex0;

uniform vec2 resolution;
uniform float scaleX;
uniform float scaleY;
uniform float blend;

uniform float angle;

// vec2 mirror(vec2 x,float sc)
// {
//   return abs(fract(x/(2.0*sc)) - 0.5)*2.0*sc; 
// }
// vec2 mirrorA(vec2 x,float sc){

// }
float tfract(float x){
  if (x>=0.)
  return x-floor(x);
  else
  return x-ceil(x);
}

float sawtooth(float x, float p){
  return abs(mod((x-p),2.0*p) - p);
}
void main()
{
  vec2 st =  gl_TexCoord[0].st;
  float _scale = max(resolution.x,resolution.y)/2.0;
    // float scale = 1.0;
    // vec2 scale = resolution;
    vec2 uv = (st - (resolution.xy/2.0) )/ _scale; // centered coords
    // float ar = resolution.x/resolution.y;
    if(scaleX<1)
      {uv.x = sawtooth(uv.x,scaleX);}
    if(scaleY<1)
      {uv.y = sawtooth(uv.y,scaleY);}
    vec2 ax  = vec2(cos(angle),sin(angle));
    vec2 nx = vec2(-ax.y,ax.x);
    uv = vec2(dot(uv,ax), dot(uv,nx));
    // float dist = length(uv);
    // uv.x = abs(tfract(uv.x*a)/a);
    
    // uv = uv/length(uv)*dist;
    // uv = mirror(uv,a*resolution.x);
    

    uv = _scale*uv+resolution.xy/2.0;
    // float a = atan(p.y,p.x);
    // float r = sqrt(dot(p,p));

    vec4 col=texture2DRect(tex0, uv);
    if(col.r+col.g+col.b > 0.1){
      col+=blend*texture2DRect(tex0, st);
    }

    gl_FragColor = col;


  }