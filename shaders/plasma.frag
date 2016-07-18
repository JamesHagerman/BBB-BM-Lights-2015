// Oldskool plasm shader. (c) Victor Korsun, bitekas@gmail.com; 1996-2013.
//
// Attribution-ShareAlike CC License.

//----------------
  const int ps = 1; // use values > 1..10 for oldskool
//----------------


void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
   float x = fragCoord.x / iResolution.x * 640.;
   float y = fragCoord.y / iResolution.y * 480.;

	if (ps > 0)
	{
	   x = float(int(x / float(ps)) * ps);
	   y = float(int(y / float(ps)) * ps);
	}

   float mov0 = x+y+sin(iGlobalTime)*10.+sin(x/90.)*70.+iGlobalTime*2.;
   float mov1 = (mov0 / 5. + sin(mov0 / 30.))/ 10. + iGlobalTime * 3.;
   float mov2 = mov1 + sin(mov1)*5. + iGlobalTime*1.0;
   float cl1 = sin(sin(mov1/4. + iGlobalTime)+mov1);
   float c1 = cl1 +mov2/2.-mov1-mov2+iGlobalTime;
   float c2 = sin(c1+sin(mov0/100.+iGlobalTime)+sin(y/57.+iGlobalTime/50.)+sin((x+y)/200.)*2.);
   float c3 = abs(sin(c2+cos((mov1+mov2+c2) / 10.)+cos((mov2) / 10.)+sin(x/80.)));

   float dc = float(16-ps);

	if (ps > 0)
	{
   		cl1 = float(int(cl1*dc))/dc;
   		c2 = float(int(c2*dc))/dc;
   		c3 = float(int(c3*dc))/dc;
	}

   fragColor = vec4( cl1,c2,c3,1.0);
}