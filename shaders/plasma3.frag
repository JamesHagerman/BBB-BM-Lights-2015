const float PI = 3.14159265;
const float OFFSET = (3.14159265*2.0)/3.0;

//converts float to RAINBOWS
//infinitely cyclable, so it still works after the per-pixel values get ridiculously huge
vec3 cget(float x){
	vec3 outp;
	outp.r = sin(x);
	outp.g = sin(x+OFFSET);
	outp.b = sin(x+OFFSET*2.0);
	outp = pow(outp,vec3(2.0));
	outp *= 0.5+0.5*sin(x/1.7);
	return outp;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	float t = iGlobalTime;
	vec2 uv = fragCoord.xy/iResolution.xy - vec2(0.5);//centred system
	uv.x *= iResolution.x/iResolution.y;//aspect correction
	uv = uv*4.0;//scale for more plasma

	//some slightly fucked up rotations for fun and things
	float xrot = uv.x*cos(t*0.2)-uv.y*sin(t*0.5);
	float yrot = uv.x*sin(t*0.3)+uv.y*cos(t*0.2)+t;

	//make some random wavy patterns
	float c1,c2;
	c1 = sin((xrot+sin(yrot*6.0+t)*0.3)*2.0)+t;
	c2 = sin(uv.y)*PI+sin(uv.x+t);

	float cfinal = c1*0.5+0.3*sin(t)
		+c2*0.5-0.3*sin(t);
	fragColor = vec4(cget(cfinal),1.0);
}