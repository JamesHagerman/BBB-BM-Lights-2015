float t = iGlobalTime;
//noise and hash funtions by iq
vec2 hash( vec2 p ) 
{                       // rand in [-1,1]
    p = vec2( dot(p,vec2(127.1,311.7)),
              dot(p,vec2(269.5,183.3)) );
    return -1. + 2.*fract(sin(p+20.)*53758.5453123);
}
float noise( in vec2 p ) 
{
    vec2 i = floor((p)), f = fract((p));
    vec2 u = f*f*(3.-2.*f);
    return mix( mix( dot( hash( i + vec2(0.,0.) ), f - vec2(0.,0.) ), 
                     dot( hash( i + vec2(1.,0.) ), f - vec2(1.,0.) ), u.x),
                mix( dot( hash( i + vec2(0.,1.) ), f - vec2(0.,1.) ), 
                     dot( hash( i + vec2(1.,1.) ), f - vec2(1.,1.) ), u.x), u.y);
}
//turbulence fuction from:
//http://developer.download.nvidia.com/SDK/9.5/Samples/samples.html
//from: 'Cg Explosion Effect'
//A lot more pdf/whitepapers there...
float turbulence(vec3 p, int octaves, float lacunarity, float gain)
{
    float sum = 0.;
    float amp = 1.;
    
    for(int i=0; i<4; i++) 
    {
        sum += amp * abs(noise(p.xy));
        p *= lacunarity;
        amp *= gain;
    }
    
    return sum;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    
    vec2 uv = fragCoord.xy / iResolution.xy;
    uv.x*=1.6;
    uv.x-=.3;
    //uv.x+=sin(t*.5)/15.+noise(vec2(t,t*.2))/5.;
    //uv.y+=cos(t)/10.;

    float tb3= turbulence(vec3((uv*(14.))-t*5.,1.0),0,.05,.7);
	
    uv=length((uv)-.5)-vec2(.5,.8+noise(vec2(uv.x,uv.y+tb3))/8.);
    
    float tb = turbulence(vec3((uv*(13.))-t,1.0),0,.05,.5); 
    float tb2= turbulence(vec3((uv*(5.))-t*.5,1.0),0,.04,.4);

	tb=tb+tb2;
	fragColor = vec4(uv+tb,0.0,1.0)*1.9;

}