// Rebb/TRSi^Paradise

float an= sin(iGlobalTime)/3.14157;
float as= sin(an);
float zoo = .23232+.38*sin(.7*iGlobalTime);
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
  vec2 position = ( fragCoord.xy / iResolution.xy *3.3 );

	float color = 0.0;
	color += sin(position.x - position.y) ;
	color += sin(iGlobalTime)* cos(sin(iGlobalTime)*position.y*position.x*sin(position.x))+.008;
	color += sin(iGlobalTime)+position.x*sin(position.y*sin(sin(tan(cos (iGlobalTime)))));
	fragColor = vec4( vec3(sin(color*color)*4.0, sin(color*color) , color )*sin(iGlobalTime+position.x/(iGlobalTime*3.14)),iGlobalTime/10.828 );

}