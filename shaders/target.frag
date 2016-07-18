const float rings = 5.0;	//exactly the number of complete white rings at any moment.
const float velocity=4.;
const float b = 0.003;		//size of the smoothed border

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 position = fragCoord.xy/iResolution.xy;
    float aspect = iResolution.x/iResolution.y;
	position.x *= aspect;
	float dist = distance(position, vec2(aspect*0.5, 0.5));
	float offset=iGlobalTime*velocity;
	float conv=rings*4.;
	float v=dist*conv-offset;
	float ringr=floor(v);
	float color=smoothstep(-b, b, abs(dist- (ringr+float(fract(v)>0.5)+offset)/conv));
	if(mod(ringr,2.)==1.)
		color=1.-color;
	fragColor = vec4(color, color, color, 1.);
}
