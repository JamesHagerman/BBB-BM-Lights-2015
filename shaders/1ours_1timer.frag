vec3 hsv2rgb_smooth( in vec3 c ) { vec3 rgb = clamp( abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0, 0.0, 1.0 ); rgb = rgb*rgb*(3.0-2.0*rgb); return c.z * mix( vec3(1.0), rgb, c.y); }
vec3 outputColor(float aFinalHue, float aFinalSaturation, float aFinalValue) { return hsv2rgb_smooth(vec3(aFinalHue, aFinalSaturation, aFinalValue)); }
float rand(vec2 co) { return fract(sin(dot(co*0.123,vec2(12.9898,78.233))) * 43758.5453); }
float convertHue(float aHue) { return aHue/360.0; }

const float PURPLE = 304.0;
const float BLUE = 239.0;
const float CYAN = 183.0;
const float GREEN = 131.0;
const float YELLOW = 62.0;
const float ORANGE = 42.0;
const float RED = 0.0;

float ROWS = 12.0;

float evaluateColor(in vec2 fragCoord, in float aCycle, in float aCycleDelay) {
//float evaluateColor(in float aRow, in vec2 fragCoord, in float aCycle) {
    float tFinalHue = 0.0;
    float iCurrentTime = iGlobalTime - aCycleDelay;
    //float iCurrentTime = iGlobalTime - (aCycle * aRow);
    float tPercentTimeUntilAllRed = iCurrentTime/aCycle;
    if (tPercentTimeUntilAllRed > (fragCoord.x/iResolution.x) + sin(iGlobalTime * 5.0)*.1 + sin(iGlobalTime * 3.0)*.1 ) {
        tFinalHue = convertHue(RED) + sin(iGlobalTime*1.1)*.075;
        if (tPercentTimeUntilAllRed > 1.0) {
            float tPercentTimeUntilAllYellow = (iCurrentTime-aCycle*12.0)/aCycle;
            if (tPercentTimeUntilAllYellow > (fragCoord.x/iResolution.x)  + sin(iGlobalTime * 5.0)*.1 + sin(iGlobalTime * 3.0)*.1 ) {
                tFinalHue = convertHue(YELLOW) + abs(sin(iGlobalTime*0.9)*.05)*-1.0;
                float tPercentageTimeUntilAllGreen = (iCurrentTime-aCycle*2.0*12.0)/aCycle;
                if (tPercentageTimeUntilAllGreen > (fragCoord.x/iResolution.x)  + sin(iGlobalTime * 5.0)*.1 + sin(iGlobalTime * 3.0)*.1 ) {
                    tFinalHue = convertHue(GREEN);
                }
            }
        }
    } else {
        tFinalHue = convertHue(BLUE) + abs(sin(iGlobalTime * .6)*.075);
    }
    return tFinalHue;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
    float tCountdownTimeInMinutes = 10.0;
    float tSecondsInAMinute = 60.0;
    float tTotalTimeInSeconds = tCountdownTimeInMinutes * tSecondsInAMinute;
    float tColors = 3.0;
    float tCycle = (tTotalTimeInSeconds/tColors)/ROWS;
    float tFinalHue = 0.0;
    float tFinalSaturation = 1.0;
    if (fragCoord.y > (iResolution.y * (11.0/ROWS))) {
        tFinalHue = evaluateColor(fragCoord, tCycle, tCycle*0.0);
    } else if (fragCoord.y > (iResolution.y * (10.0/ROWS))) {
        tFinalHue = evaluateColor(fragCoord, tCycle, tCycle*1.0);
    } else if (fragCoord.y > (iResolution.y * (9.0/ROWS))) {
        tFinalHue = evaluateColor(fragCoord, tCycle, tCycle*2.0);
    } else if (fragCoord.y > (iResolution.y * (8.0/ROWS))) {
        tFinalHue = evaluateColor(fragCoord, tCycle, tCycle*3.0);
    } else if (fragCoord.y > (iResolution.y * (7.0/ROWS))) {
        tFinalHue = evaluateColor(fragCoord, tCycle, tCycle*4.0);
    } else if (fragCoord.y > (iResolution.y * (6.0/ROWS))) {
        tFinalHue = evaluateColor(fragCoord, tCycle, tCycle*5.0);
    } else if (fragCoord.y > (iResolution.y * (5.0/ROWS))) {
        tFinalHue = evaluateColor(fragCoord, tCycle, tCycle*6.0);
    } else if (fragCoord.y > (iResolution.y * (4.0/ROWS))) {
        tFinalHue = evaluateColor(fragCoord, tCycle, tCycle*7.0);
    } else if (fragCoord.y > (iResolution.y * (3.0/ROWS))) {
        tFinalHue = evaluateColor(fragCoord, tCycle, tCycle*8.0);
    } else if (fragCoord.y > (iResolution.y * (2.0/ROWS))) {
        tFinalHue = evaluateColor(fragCoord, tCycle, tCycle*9.0);
    } else if (fragCoord.y > (iResolution.y * (1.0/ROWS))) {
        tFinalHue = evaluateColor(fragCoord, tCycle, tCycle*10.0);
    } else {
        tFinalHue = evaluateColor(fragCoord, tCycle, tCycle*11.0);
    }

    float tPercentageTimeUntilDone = iGlobalTime/(tCycle*3.0*12.0);
    if (tPercentageTimeUntilDone > 1.0 ) {
        tFinalSaturation = abs(sin(iGlobalTime)) * .5;
    }

	fragColor = vec4(outputColor(tFinalHue, tFinalSaturation, 1.0), 1.0);
}

/*
for (float i = 0.0; i < 12.0; i++) {
        if (fragCoord.y > (iResolution.y * (11.0/ROWS)) &&
            fragCoord.y < (iResolution.y * (12.0/ROWS))) { //trying to figure out how to mod with 'i'
            tFinalHue = evaluateColor(i, fragCoord, tCycle); //???
            break;
        }
    }
*/

/* previous non-for-looped code

*/