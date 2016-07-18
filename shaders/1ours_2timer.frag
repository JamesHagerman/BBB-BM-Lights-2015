vec3 hsv2rgb_smooth( in vec3 c ) { vec3 rgb = clamp( abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0, 0.0, 1.0 ); rgb = rgb*rgb*(3.0-2.0*rgb); return c.z * mix( vec3(1.0), rgb, c.y); }
vec3 outputColor(float aFinalHue) { return hsv2rgb_smooth(vec3(aFinalHue, 1.0, 1.0)); }
float rand(vec2 co) { return fract(sin(dot(co*0.123,vec2(12.9898,78.233))) * 43758.5453); }
float convertHue(float aHue) { return aHue/360.0; }

const float PURPLE = 304.0;
const float BLUE = 239.0;
const float CYAN = 183.0;
const float GREEN = 131.0;
const float YELLOW = 62.0;
const float ORANGE = 42.0;
const float RED = 0.0;

float evaluateColor(in vec2 fragCoord, in float aEndOfCycle, in float aOffset) {
    float tFinalHue = 0.0;
    //float tEndOfCycle = 6.0;
    float iCurrentTime = iGlobalTime - aOffset;
    float tPercentTimeUntilAllRed = iCurrentTime/aEndOfCycle;
    if (tPercentTimeUntilAllRed > (fragCoord.x/iResolution.x)) {
        tFinalHue = convertHue(RED);
        if (tPercentTimeUntilAllRed > 1.0) {
            float tPercentTimeUntilAllYellow = (iCurrentTime-aEndOfCycle)/aEndOfCycle;
            if (tPercentTimeUntilAllYellow > (fragCoord.x/iResolution.x)) {
                tFinalHue = convertHue(YELLOW);
                float tPercentageTimeUntilAllGreen = (iCurrentTime-aEndOfCycle*2.0)/aEndOfCycle;
                if (tPercentageTimeUntilAllGreen > (fragCoord.x/iResolution.x)) {
                    tFinalHue = convertHue(GREEN);
                }
            }
        }

    } else {
        tFinalHue = convertHue(BLUE);
    }
    return tFinalHue;
}

//Make Time-Tween Go Red, Orange, Yellow, Then Blinking Green In Last Minute
void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
    float tFinalHue = 0.;

    float tCountdownTimeInMinutes = 1.;
    float tSecondsInAMinute = 60.0;
    float tTotalTimeInSeconds = tCountdownTimeInMinutes * tSecondsInAMinute;
    float tPercentTimeUntilLiftoff = iGlobalTime/tTotalTimeInSeconds;

    float tCycleTime = 6.0;
    float tCycleDelay = 6.0;


    for (int i = 0; i < 6; i++)
	{
	}


   	//tFinalHue = evaluateColor(fragCoord, 0.0);
    if (fragCoord.y > (iResolution.y * (11.0/12.0))) {
        tFinalHue = evaluateColor(fragCoord, tCycleTime, 0.0);
    } else if (fragCoord.y > (iResolution.y * (10.0/12.0))) {
        tFinalHue = evaluateColor(fragCoord, tCycleTime, tCycleDelay);
    } else if (fragCoord.y > (iResolution.y * (9.0/12.0))) {
        tFinalHue = evaluateColor(fragCoord, tCycleTime, tCycleDelay*2.0);
    } else if (fragCoord.y > (iResolution.y * (8.0/12.0))) {
        tFinalHue = evaluateColor(fragCoord, tCycleTime, tCycleDelay*3.0);
    } else if (fragCoord.y > (iResolution.y * (7.0/12.0))) {
        tFinalHue = evaluateColor(fragCoord, tCycleTime, tCycleDelay*4.0);
    } else if (fragCoord.y > (iResolution.y * (6.0/12.0))) {
        tFinalHue = evaluateColor(fragCoord, tCycleTime, tCycleDelay*5.0);
    } else if (fragCoord.y > (iResolution.y * (5.0/12.0))) {
        tFinalHue = evaluateColor(fragCoord, tCycleTime, tCycleDelay*6.0);
    } else if (fragCoord.y > (iResolution.y * (4.0/12.0))) {
        tFinalHue = evaluateColor(fragCoord, tCycleTime, tCycleDelay*7.0);
    } else if (fragCoord.y > (iResolution.y * (3.0/12.0))) {
        tFinalHue = evaluateColor(fragCoord, tCycleTime, tCycleDelay*8.0);
    } else if (fragCoord.y > (iResolution.y * (2.0/12.0))) {
        tFinalHue = evaluateColor(fragCoord, tCycleTime, tCycleDelay*9.0);
    } else if (fragCoord.y > (iResolution.y * (1.0/12.0))) {
        tFinalHue = evaluateColor(fragCoord, tCycleTime, tCycleDelay*10.0);
    } else {
        tFinalHue = evaluateColor(fragCoord, tCycleTime, tCycleDelay*11.0);
    }

	fragColor = vec4(outputColor(tFinalHue), 1.0);
}





void otherCode() {
    float tFinalHue = 0.;
    float tCountdownTimeInMinutes = 1.;
    float tSecondsInAMinute = 60.0;
    float tTotalTimeInSeconds = tCountdownTimeInMinutes * tSecondsInAMinute;
    float tPercentTimeUntilLiftoff = iGlobalTime/tTotalTimeInSeconds;
    if (tPercentTimeUntilLiftoff < .26) {
        //do red sequence
        tFinalHue = convertHue(RED);
    }

    if (tPercentTimeUntilLiftoff >= .26 && tPercentTimeUntilLiftoff < .52) {
    	//do orange sequence
        tFinalHue = convertHue(ORANGE);
    }
    if (tPercentTimeUntilLiftoff >= .52 && tPercentTimeUntilLiftoff < .8) {
    	//do yellow sequence
        tFinalHue = convertHue(YELLOW);
    }
    if (tPercentTimeUntilLiftoff >= .8) {
    	//do green sequence
        tFinalHue = convertHue(GREEN);
    }
}