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
        fragColor = vec4(hsv2rgb_smooth(vec3(tFinalHue, 1.0, 0.0)), 1.0);
    } else {
        fragColor = vec4(outputColor(tFinalHue), 1.0);
    }


}


//vec3 hsv2rgb_smooth( in vec3 c ) { vec3 rgb = clamp( abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0, 0.0, 1.0 ); rgb = rgb*rgb*(3.0-2.0*rgb); return c.z * mix( vec3(1.0), rgb, c.y); }
//vec3 outputColor(float aFinalHue) { return hsv2rgb_smooth(vec3(aFinalHue, 1.0, 1.0)); }
//float rand(vec2 co) { return fract(sin(dot(co*0.123,vec2(12.9898,78.233))) * 43758.5453); }
//float convertHue(float aHue) { return aHue/360.0; }
//
//const float PURPLE = 304.0;
//const float BLUE = 239.0;
//const float CYAN = 183.0;
//const float GREEN = 131.0;
//const float YELLOW = 62.0;
//const float ORANGE = 42.0;
//const float RED = 0.0;
//
//float evaluateColor(in vec2 fragCoord, in float aEndOfCycle, in float aOffset) {
//    float tFinalHue = 0.0;
//    //float tEndOfCycle = 6.0;
//    float iCurrentTime = iGlobalTime - aOffset;
//    float tPercentTimeUntilAllRed = iCurrentTime/aEndOfCycle;
//    if (tPercentTimeUntilAllRed > (fragCoord.x/iResolution.x)) {
//        tFinalHue = convertHue(RED);
//        if (tPercentTimeUntilAllRed > 1.0) {
//            float tPercentTimeUntilAllYellow = (iCurrentTime-aEndOfCycle)/aEndOfCycle;
//            if (tPercentTimeUntilAllYellow > (fragCoord.x/iResolution.x)) {
//                tFinalHue = convertHue(YELLOW);
//                float tPercentageTimeUntilAllGreen = (iCurrentTime-aEndOfCycle*2.0)/aEndOfCycle;
//                if (tPercentageTimeUntilAllGreen > (fragCoord.x/iResolution.x)) {
//                    tFinalHue = convertHue(GREEN);
//                }
//            }
//        }
//
//    } else {
//        tFinalHue = convertHue(BLUE);
//    }
//    return tFinalHue;
//}
//
////Make Time-Tween Go Red, Orange, Yellow, Then Blinking Green In Last Minute
//void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
//    float tFinalHue = 0.;
//
//    float tCountdownTimeInMinutes = 1.;
//    float tSecondsInAMinute = 60.0;
//    float tTotalTimeInSeconds = tCountdownTimeInMinutes * tSecondsInAMinute;
//    float tPercentTimeUntilLiftoff = iGlobalTime/tTotalTimeInSeconds;
//
//    float tCycleTime = 6.0;
//    float tCycleDelay = 6.0;
//
//
//    for (int i = 0; i < 6; i++)
//	{
//	}
//
//
//   	//tFinalHue = evaluateColor(fragCoord, 0.0);
//    if (fragCoord.y > (iResolution.y * (11.0/12.0))) {
//        tFinalHue = evaluateColor(fragCoord, tCycleTime, 0.0);
//    } else if (fragCoord.y > (iResolution.y * (10.0/12.0))) {
//        tFinalHue = evaluateColor(fragCoord, tCycleTime, tCycleDelay);
//    } else if (fragCoord.y > (iResolution.y * (9.0/12.0))) {
//        tFinalHue = evaluateColor(fragCoord, tCycleTime, tCycleDelay*2.0);
//    } else if (fragCoord.y > (iResolution.y * (8.0/12.0))) {
//        tFinalHue = evaluateColor(fragCoord, tCycleTime, tCycleDelay*3.0);
//    } else if (fragCoord.y > (iResolution.y * (7.0/12.0))) {
//        tFinalHue = evaluateColor(fragCoord, tCycleTime, tCycleDelay*4.0);
//    } else if (fragCoord.y > (iResolution.y * (6.0/12.0))) {
//        tFinalHue = evaluateColor(fragCoord, tCycleTime, tCycleDelay*5.0);
//    } else if (fragCoord.y > (iResolution.y * (5.0/12.0))) {
//        tFinalHue = evaluateColor(fragCoord, tCycleTime, tCycleDelay*6.0);
//    } else if (fragCoord.y > (iResolution.y * (4.0/12.0))) {
//        tFinalHue = evaluateColor(fragCoord, tCycleTime, tCycleDelay*7.0);
//    } else if (fragCoord.y > (iResolution.y * (3.0/12.0))) {
//        tFinalHue = evaluateColor(fragCoord, tCycleTime, tCycleDelay*8.0);
//    } else if (fragCoord.y > (iResolution.y * (2.0/12.0))) {
//        tFinalHue = evaluateColor(fragCoord, tCycleTime, tCycleDelay*9.0);
//    } else if (fragCoord.y > (iResolution.y * (1.0/12.0))) {
//        tFinalHue = evaluateColor(fragCoord, tCycleTime, tCycleDelay*10.0);
//    } else {
//        tFinalHue = evaluateColor(fragCoord, tCycleTime, tCycleDelay*11.0);
//    }
//
//	fragColor = vec4(outputColor(tFinalHue), 1.0);
//}
//
//
//
//
//
//void otherCode() {
//    float tFinalHue = 0.;
//    float tCountdownTimeInMinutes = 1.;
//    float tSecondsInAMinute = 60.0;
//    float tTotalTimeInSeconds = tCountdownTimeInMinutes * tSecondsInAMinute;
//    float tPercentTimeUntilLiftoff = iGlobalTime/tTotalTimeInSeconds;
//    if (tPercentTimeUntilLiftoff < .26) {
//        //do red sequence
//        tFinalHue = convertHue(RED);
//    }
//
//    if (tPercentTimeUntilLiftoff >= .26 && tPercentTimeUntilLiftoff < .52) {
//    	//do orange sequence
//        tFinalHue = convertHue(ORANGE);
//    }
//    if (tPercentTimeUntilLiftoff >= .52 && tPercentTimeUntilLiftoff < .8) {
//    	//do yellow sequence
//        tFinalHue = convertHue(YELLOW);
//    }
//    if (tPercentTimeUntilLiftoff >= .8) {
//    	//do green sequence
//        tFinalHue = convertHue(GREEN);
//    }
//}
