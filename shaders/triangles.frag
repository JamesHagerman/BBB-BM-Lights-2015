#define HARMONICS 32.0

// http://en.wikipedia.org/wiki/Triangle_wave

float wave( vec2 uv, float time){

	float t = (time/16.0-uv.x) * 8.0;

	float x=sin(t);

	for (float i = 1.0; i <= HARMONICS; i+=1.0) {

		float h = i * 2.0 + 1.0;
		float wave = sin(t*h)/pow(h,2.0);

		if (mod(i,2.0) == 0.0) x += wave;
		else x -= wave;

	}

	x = x/2.0;

	float y = uv.y*2.0-1.0;

	return (x < y) ? 1.0 : 0.0;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 uv = fragCoord.xy / iResolution.xy;
	fragColor = vec4( vec3(wave(uv, iGlobalTime)), 1.0);
}


