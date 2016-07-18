// Echophon poured some soap onto this.
// Made into third eye tye-dye by Cale Bradbury - @netgrind 2015
// Edited version of https://www.shadertoy.com/view/MsfGzM Created by inigo quilez - iq/2013
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

float i = iGlobalTime;

float f(vec3 p)
{
    p.z-=i*.05;
    return length(cos(p)-.001*cos(9.*(p.z*.09*p.x-p.y)+i*2.0))-(0.9+sin(iGlobalTime)*.1);
}

float g(vec3 p)
{
    p.x-=i*.08;
    p.y-=i*.13;
    return 1./(1.-p.x*p.y*p.z);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{

    vec3 d = .5-vec3(fragCoord,1.)/iResolution.x;
    d.y+=sin(0.01*i);
    vec3 o=d;
    float l = length(d.xyz)*5.;
    float a = atan(d.y,d.x)*.2;
    o.xy*= mat2(cos(i+sin(a+i))*0.1, 1.-sin(i*.05+a*l)*2.0, sin(i+a),cos(i*d.z+l)+2.0);
    for(int i=0;i<4;i++){
        o+=f(o)*d-g(d);
        o*=-d-f(o)-g(d.zxy);
    }
    o.z = length(o*d);
    fragColor=vec4(sin(i+abs((o+d)+length(o.xy*step(o.z,700.0))))*.3+.7,1.0);
}