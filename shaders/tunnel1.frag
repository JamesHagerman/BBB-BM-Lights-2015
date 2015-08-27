#define S sin(p.z*.2)*vec4(1,2,0,0)
void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
    vec2 uv = fragCoord.xy / iResolution.xy;
    vec4 p = vec4(fragCoord,0.,1.)/iResolution.xyxy-.5, d=p, t;
    p.z += iGlobalTime*4.;
    p -= S;
    for(float i=2.; i>0.; i-=.01)
    {
        t = abs(mod(p + S, 8.)-4.);
        t = min(t.x, t.y)-texture2D( cogl_sampler, vec2(uv.x,0.25) );
        fragColor = p.wxxw*.02+i/p.w;
        if(t.x<.01) break;
        p -= d*t.x*.2;
     }
}
