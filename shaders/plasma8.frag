void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 uv = fragCoord.xy / iResolution.xy;
	float time = iGlobalTime;
	float final = (sin(uv.x*32.0+sin(sin((uv.y)*32.0)+sin(time*1.0)*4.0+time*3.0)+sin(uv.x*64.0)+sin(uv.y*32.0+time*4.0)+time*4.0-sin(uv.y*2.0-time)*8.0+time));
	fragColor = vec4(abs(final),final,final,1.0);
}