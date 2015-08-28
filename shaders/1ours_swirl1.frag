


vec3 hsv2rgb_smooth( in vec3 c )
{
    vec3 rgb = clamp( abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0, 0.0, 1.0 );
	rgb = rgb*rgb*(3.0-2.0*rgb); // cubic smoothing
	return c.z * mix( vec3(1.0), rgb, c.y);
}

vec3 pal( in float t, in vec3 a, in vec3 b, in vec3 c, in vec3 d )
{
    return a + b*cos( 6.28318*(c*t+d) );
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 uv = fragCoord.xy / iResolution.xy;
    vec2 mv = iMouse.xy/iResolution.xy;

    //uv.x += sin(iGlobalTime*0.5);
    //uv.x -= 1.;

    float val = mv.x*0.5;
    float color = (iGlobalTime+20.)+val*atan((uv.y-mv.y), (uv.x-mv.x));
    //uv.y += atan (uv.x, uv.y);

	//vec3 col = pal( uv.x, vec3(0.5,0.5,0.5),vec3(0.5,0.5,0.5),vec3(1.0,1.0,1.0),vec3(0.0,0.33,0.67) );
    //col = pal( uv.x, vec3(0.5,0.5,0.5),vec3(20.5,20.5,20.5),vec3(1.0,1.0,1.0),vec3(0.0,0.33,0.67) );
    //col = pal( uv.x, vec3(2.0*sin(iGlobalTime*2.0),2.0*cos(iGlobalTime*2.0),0.5),vec3(5.5,0.5,0.5),vec3(1.5,1.0,1.0),vec3(0.0,0.33,0.67) );

    vec3 hsvOut = hsv2rgb_smooth(vec3(color, 1.0, 1.0));

	fragColor = vec4(hsvOut, 1.0);
}