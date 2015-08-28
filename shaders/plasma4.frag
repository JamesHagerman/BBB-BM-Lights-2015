#define PI 3.1415926535897932384626433832795

float plasmaGeneration(void)
{
	float v = 0.0;
	vec2 u_k = vec2(0.003,0.02);
    vec2 c = gl_FragCoord.xy * u_k - u_k/2.0;
    v += sin(c.x * 7.4 + iGlobalTime);
	v += sin(c.y * sin((c.x + iGlobalTime)/0.7) + c.x * cos(iGlobalTime - c.y) + iGlobalTime);
	v += 0.0;
	c.x += sin(iGlobalTime / 10.21235);
	c.y += cos(iGlobalTime / 22.12361);
	v += sin(sqrt(4.0 * (c.x * c.x +c.y * c.y) + 1.0) + iGlobalTime);
	return v;
}


void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	float v = plasmaGeneration();
	vec3 col = vec3(sin(v) + 2.0, sin(PI * v), cos(PI * v));
    fragColor = vec4(col*.5 + .5, 1.0);
}