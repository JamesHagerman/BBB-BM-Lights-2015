void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    //Get uvs
	vec2 uv = vec2(iResolution.x/iResolution.y,1.0);
    uv *= (-1.0 + 2.0*fragCoord.xy / iResolution.xy);

    //Save old uvs
    vec2 origUvs = uv;

    //Rotation Angle
    float a = sin(iGlobalTime*0.05)*4.2;

    //Create Rotation Matrix, Rot uvs by it
    float cosa = cos(a);
    float sina = sin(a);
    uv *= mat2(cosa, sina, -sina, cosa);

    //Warp Uvs
    float warp = abs(uv.y+2.6)*(uv.y*.3+uv.x)*uv.x;
    uv *= 3.0+1.9*warp;

    //Move uvs iwth time
    uv.x += iGlobalTime*2.0;
    uv.y += iGlobalTime*2.0;

    //scale up
    uv *= 2.0;

    //Gen imagry
    uv.y = abs(1. * 1. * sin(uv.y));
    uv.x = abs(1. * 1. * sin(uv.x + iGlobalTime*10.0));

    //Makes smaller balls!
    //uv = pow(uv, vec2(1.0, 4.0));

    fragColor = vec4(uv.y+uv.x+0.0, (uv.y+uv.x)*1.0 - warp, 0.25+uv.y, 1.0) * vec4(.5 - (abs(clamp(warp, 0., 1.)))*0.05, 1., 0., 1.);
}