
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 uv = fragCoord.xy / iResolution.xy;
    float pi = 3.14;
    float color = 0.0;
    color += 0.7*sin(0.5*uv.x + iGlobalTime/5.0);
    color += 3.0*sin(1.6*uv.y + iGlobalTime/5.0);
    color += 1.0*sin(10.0*(uv.y * sin(iGlobalTime/2.0) + uv.x * cos(iGlobalTime/5.0)) + iGlobalTime/2.0);
    float cx = uv.x + 0.5*sin(iGlobalTime/2.0);
    float cy = uv.y + 0.5*cos(iGlobalTime/4.0);
    color += 0.4*sin(sqrt(100.0*cx*cx + 100.0*cy*cy + 1.0) + iGlobalTime);
    color += 0.9*sin(sqrt(75.0*cx*cx + 25.0*cy*cy + 1.0) + iGlobalTime);
    color += -1.4*sin(sqrt(256.0*cx*cx + 25.0*cy*cy + 1.0) + iGlobalTime);
    color += 0.3 * sin(0.5*uv.y + uv.x + sin(iGlobalTime));
    float r = 0.5+0.5*sin(color * pi);
    float g = 0.5+0.5*sin(color * pi + 2.0 * pi / 3.0);
    float b = 0.5+0.5*sin(color + 4.0 * pi / 3.0);
	fragColor = vec4(r,g,b,1.0);
}