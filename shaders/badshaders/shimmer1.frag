// Ben Weston - 2013
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

const float tau = 6.28318530717958647692;

// Gamma correction
#define GAMMA (2.2)

vec3 ToLinear( in vec3 col )
{
	// simulate a monitor, converting colour values into light values
	return pow( col, vec3(GAMMA) );
}

vec3 ToGamma( in vec3 col )
{
	// convert back into colour values, so the correct light will come out of the monitor
	return pow( col, vec3(1.0/GAMMA) );
}


vec2 Noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);
	f = f*f*(3.0-2.0*f);
//	vec3 f2 = f*f; f = f*f2*(10.0-15.0*f+6.0*f2);

	vec2 uv = (p.xy+vec2(37.0,17.0)*p.z) + f.xy;

#if (0)
	vec4 rg = texture2D( cogl_sampler, (uv+0.5)/256.0, -100.0 );
#else
	// on some hardware interpolation lacks precision
	vec4 rg = mix( mix(
				texture2D( cogl_sampler, (floor(uv)+0.5)/256.0, -100.0 ),
				texture2D( cogl_sampler, (floor(uv)+vec2(1,0)+0.5)/256.0, -100.0 ),
				fract(uv.x) ),
				  mix(
				texture2D( cogl_sampler, (floor(uv)+vec2(0,1)+0.5)/256.0, -100.0 ),
				texture2D( cogl_sampler, (floor(uv)+1.5)/256.0, -100.0 ),
				fract(uv.x) ),
				fract(uv.y) );
#endif			  

	return mix( rg.yw, rg.xz, f.z );
}


void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 uv = (fragCoord.xy-.5*iResolution.xy) / iResolution.x;
	
	vec2 blob = Noise( vec3(uv.x,uv.y*sqrt(3.0)*.5,uv.y*.5)*4.0 + iGlobalTime*vec3(0,-.1,.1) );
	
	const vec3 ink1 = vec3(.1,.9,.8);
	const vec3 ink2 = vec3(.9,.1,.6);
	
	vec3 col1 = pow(ink1,vec3(4.0*sqrt(max(0.0,(blob.x-.6)*2.0))));
	vec3 col2 = pow(ink2,vec3(4.0*sqrt(max(0.0,(blob.y-.6)*2.0))));
	
	fragColor = vec4(ToGamma(col1*col2),1);
}
