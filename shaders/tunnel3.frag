void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 uv = fragCoord.xy / iResolution.xy;

	float n = iGlobalTime;
	float x = uv.x;
	float y = uv.y;

	float xcir = x-0.5;
	float ycir = y-0.5;

	float circle = ((xcir*xcir)+(ycir*ycir)/3.0);
	float tunnel = sqrt(circle*0.25)/circle*4.0+n*8.0;
	float wally = sin(atan(xcir/ycir*1.5)*8.0+cos(n/2.0)*16.0*sin(tunnel+n)/4.0+sin(n/3.2)*32.0);

	float shade=clamp(circle*16.0,0.0,1.0);
	float full=(sin(tunnel)*wally);
	fragColor = vec4(full*shade*1.4,(full-0.25)*shade,abs(full+0.5)*shade,1.0);
}