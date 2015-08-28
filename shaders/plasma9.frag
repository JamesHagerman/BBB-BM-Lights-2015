void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 uv = (fragCoord.xy) / max(iResolution.x, iResolution.y) + vec2(1., 1.);
	uv *= 20.;
	float c = 0.;
	for (float i=0.; i<6.; i++)
	{
		float a = i*2.718281828;
		c += cos((iGlobalTime/12.94427191)+(sin(a)*uv.x+cos(a)*uv.y));
	}
	c = mod(c, 2.0);
	if (c > 1.) c = 2.-c;
	//c = (c > 0.5) ? 1. : 0.; // TODO
	c *= 3.0;
	//c = c/2.0+0.5;
	fragColor = vec4(vec3(sin(c), sin(c+1.047197551), sin(c-1.047197551)), 1.0);
}