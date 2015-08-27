#define alpha 0.0
#define beta 10.0

vec4 getCornerColors(vec2 coord)
{
    vec4 cornerColors[4];

    cornerColors[0] = vec4(1.0, 0, 0, 1.0);
    cornerColors[1] = vec4(0, 0, 1.0, 1.0);
    cornerColors[2] = vec4(0, 1.0, 0.0, 1.0);
    cornerColors[3] = vec4(1.0, 1.0, 0.0, 1.0);

    vec2 cornerCoords[4];
    cornerCoords[0] = vec2(0);
    cornerCoords[1] = vec2(1, 0);
    cornerCoords[2] = vec2(1);
    cornerCoords[3] = vec2(0, 1);


	vec4 result = vec4(0.0);

	float totalArea = dot(iResolution.xy, iResolution.xy);

	for(int i = 0; i < 4; i++)
	{
		vec2 cCoord = cornerCoords[i] * iResolution.xy;

		vec2 diff = coord - cCoord;

		float area = dot(diff, diff);

		result += ((totalArea - area) / totalArea) * cornerColors[i];
	}

	return result;
}

vec4 spiral4(vec2 coord)
{
	float alpha_t = alpha - iGlobalTime * 50.0;

	float x = coord.x;
	float y = coord.y;

	float r = sqrt(dot(coord, coord));

	float phi = atan(y, x);

	float phi_r = (r - alpha_t) / beta;

	float r_phi = alpha_t + (beta * phi);

	float remainder = abs(cos(phi) - cos(phi_r));

	if (remainder < 0.5)
	{
		return vec4(vec3(0), 1.0);
	}
	else
	{
		return vec4(vec3(remainder), 1.0);
	}
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 uv = fragCoord.xy - (iResolution.xy * 0.5);
	//fragColor = spiral4(uv) * vec4(uv,0.5+0.5*sin(iGlobalTime),1.0);
    fragColor = spiral4(uv) * (getCornerColors(fragCoord.xy) * 0.5);
}