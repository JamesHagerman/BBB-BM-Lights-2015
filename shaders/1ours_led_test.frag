vec3 hsv2rgb_smooth( in vec3 c ) { vec3 rgb = clamp( abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0, 0.0, 1.0 ); rgb = rgb*rgb*(3.0-2.0*rgb); return c.z * mix( vec3(1.0), rgb, c.y); }
vec3 outputColor(float aFinalHue) { return hsv2rgb_smooth(vec3(aFinalHue, 1.0, 1.0)); }

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy;

    float tFinalHue = 0.0;
	if (uv.y*12. > 11.0) {
        tFinalHue = 12.0/12.0;
    } else if (uv.y*12. > 10.) {
        tFinalHue = 11.0/12.0;
    } else if (uv.y*12. > 9.0) {
        tFinalHue = 10.0/12.0;
    } else if (uv.y*12. > 8.0) {
        tFinalHue = 9.0/12.0;
    } else if (uv.y*12. > 7.0) {
        tFinalHue = 8.0/12.0;
    } else if (uv.y*12. > 6.0) {
        tFinalHue = 7.0/12.0;
    } else if (uv.y*12. > 5.0) {
        tFinalHue = 6.0/12.0;
    } else if (uv.y*12. > 4.0) {
        tFinalHue = 5.0/12.0;
    } else if (uv.y*12. > 3.0) {
        tFinalHue = 4.0/12.0;
    } else if (uv.y*12. > 2.0) {
        tFinalHue = 3.0/12.0;
    } else if (uv.y*12. > 1.0) {
        tFinalHue = 2.0/12.0;
    } else {
        tFinalHue = 1.0/12.0;
    }

    float mouseY = iMouse.y/iResolution.y;

    if (uv.y > mouseY) {
        fragColor = vec4(hsv2rgb_smooth(vec3(tFinalHue, 1.0, 0.5)), 1.0);
    } else {
        fragColor = vec4(outputColor(tFinalHue), 1.0);
    }


}