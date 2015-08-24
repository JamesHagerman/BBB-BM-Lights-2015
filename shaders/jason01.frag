// Created by inigo quilez - iq/2013
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
//
//float f(vec3 p)
//{
//	p.z+=iGlobalTime;return length(.05*cos(9.*p.y*p.x)+cos(p)-.1*cos(9.*(p.z+.3*p.x-p.y)))-1.;
//}
//void mainImage( out vec4 c, vec2 p )
//{
//    vec3 d=.5-vec3(p,1)/iResolution.x,o=d;for(int i=0;i<99;i++)o+=f(o)*d;
//    c=vec4(abs(f(o-d)*vec3(0,.1,.2)+f(o-.6)*vec3(.2,.1,0))*(10.-o.z),1);
//}
vec3 hsv2rgb_smooth( in vec3 c ) { vec3 rgb = clamp( abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0, 0.0, 1.0 ); rgb = rgb*rgb*(3.0-2.0*rgb); return c.z * mix( vec3(1.0), rgb, c.y); }
vec3 outputColor(float aFinalHue) { return hsv2rgb_smooth(vec3(aFinalHue, 1.0, 1.0)); }

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy;

    float tFinalHue = 0.0;
	if (uv.y > 11.0/12.0) {
        tFinalHue = 12.0/12.0;
    } else if (uv.y > 10.0/12.0) {
        tFinalHue = 11.0/12.0;
    } else if (uv.y > 9.0/12.0) {
        tFinalHue = 10.0/12.0;
    } else if (uv.y > 8.0/12.0) {
        tFinalHue = 9.0/12.0;
    } else if (uv.y > 7.0/12.0) {
        tFinalHue = 8.0/12.0;
    } else if (uv.y > 6.0/12.0) {
        tFinalHue = 7.0/12.0;
    } else if (uv.y > 5.0/12.0) {
        tFinalHue = 6.0/12.0;
    } else if (uv.y > 4.0/12.0) {
        tFinalHue = 5.0/12.0;
    } else if (uv.y > 3.0/12.0) {
        tFinalHue = 4.0/12.0;
    } else if (uv.y > 2.0/12.0) {
        tFinalHue = 3.0/12.0;
    } else if (uv.y > 1.0/12.0) {
        tFinalHue = 2.0/12.0;
    } else {
        tFinalHue = 1.0/12.0;
    }

    float mouseX = iMouse.x/iResolution.x;

    if (uv.y > mouseX) {
        fragColor = vec4(hsv2rgb_smooth(vec3(tFinalHue, 1.0, 0.0)), 1.0);
    } else {
        fragColor = vec4(outputColor(tFinalHue), 1.0);
    }


}