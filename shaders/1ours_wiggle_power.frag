vec3 hsv2rgb_smooth(in vec3 c) {vec3 rgb=clamp(abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0,0.0,1.0);rgb=rgb*rgb*(3.0-2.0*rgb);return c.z*mix(vec3(1.0),rgb,c.y);}
//click/drag mouse on X-axis, change colors
//click/drag mouse on Y-axis, change amount of color
void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
    float tCurrentMouseXPercent = iMouse.x / iResolution.x;
    float tCurrentMouseYPercent = iMouse.y / iResolution.y;
    float tFinalHue = tCurrentMouseXPercent;
    float tFinalValue = tCurrentMouseYPercent;
	fragColor = vec4(hsv2rgb_smooth(vec3(tFinalHue, 1.0, tFinalValue)), 1.0);
}