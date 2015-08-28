#define time iGlobalTime/1.5

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 uv = fragCoord.xy / iResolution.xy;
    uv.x *= iResolution.x / iResolution.y;
    uv.y -= .5;

    vec3 col = vec3(1.);

    for (int i = 0; i <8; ++i)
    {
        float y = cos(uv.x + time * 3. + float(i)/4.) / 2.8;
        float f = smoothstep(.05, .1, abs(uv.y - y));
        col.g *= f;

        float g = smoothstep(.06, .101, abs(uv.y - y));
        col.g = mix(col.g, g, .22);
    }


	fragColor = vec4(col, 1.0);
}