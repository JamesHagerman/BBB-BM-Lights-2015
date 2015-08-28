//Da pie is useless for this one, it's still fun to write it down by remembering the numbers.
#define PI 3.14159265358979323846264

float midsin(float val) {
	return (sin(val)/2.)+0.5;
}
float midcos(float val) {
	return (cos(val)/2.)+0.5;
}
vec3 rgb(int val1, int val2, int val3) {
    return vec3(float(val1),float(val2),float(val3))/255.;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    float time = iGlobalTime;
	vec2 position = fragCoord.xy / iResolution.xy;
    vec2 uv = vec2(((position.x-0.5)*(iResolution.x/iResolution.y))+0.5,position.y);

    vec2 point_a = vec2(midsin(time),0.4);
    vec3 color_a = rgb(255,128,0);

    vec2 point_b = vec2(midcos(time/3.),midsin(time/3.));
    vec3 color_b = rgb(100,255,0);

    vec2 point_c = vec2(midsin(time/2.53),0.8);
    vec3 color_c = rgb(0,120,255);

    vec2 point_d = vec2(midsin(time/1.4)/2.,midcos(time/1.4)/2.);
    vec3 color_d = rgb(200,0,255);

    vec3 output_colors = vec3(0.);

    if (min(min(min(length(uv-point_a),length(uv-point_b)),length(uv-point_c)),length(uv-point_d)) == length(uv-point_a)) {
        output_colors = color_a;
    }
    if (min(min(min(length(uv-point_a),length(uv-point_b)),length(uv-point_c)),length(uv-point_d)) == length(uv-point_b)) {
        output_colors = color_b;
    }
    if (min(min(min(length(uv-point_a),length(uv-point_b)),length(uv-point_c)),length(uv-point_d)) == length(uv-point_c)) {
        output_colors = color_c;
    }
    if (min(min(min(length(uv-point_a),length(uv-point_b)),length(uv-point_c)),length(uv-point_d)) == length(uv-point_d)) {
        output_colors = color_d;
    }

    float circle_size = 100.;
    float circle_1 = 1.-clamp(floor(length(uv-point_a)*circle_size),0.,1.);
    float circle_2 = 1.-clamp(floor(length(uv-point_b)*circle_size),0.,1.);
    float circle_3 = 1.-clamp(floor(length(uv-point_c)*circle_size),0.,1.);
    float circle_4 = 1.-clamp(floor(length(uv-point_d)*circle_size),0.,1.);
    float all_circles = circle_1+circle_2+circle_3+circle_4;

	fragColor = vec4(output_colors-all_circles,1.0);
}