//uniform float iGlobalTime;
//uniform vec2 iResolution;
//uniform vec2 iMouse;

void mainImage(out vec4 fragColor, in vec2 fragCoord) {
   vec2 uv = fragCoord.xy / iResolution.xy;
   fragColor = vec4(uv.x, uv.y, 0.5+0.5*sin(iGlobalTime), 1.0);
}

//void main(void) {
//   vec4 outFragColor = vec4(1.0,0.5,0,0);
//   vec2 inFragCoord = vec2(cogl_tex_coord_in[0].x*iResolution.x, cogl_tex_coord_in[0].y*iResolution.y);
//   mainImage(outFragColor, inFragCoord);
//   cogl_color_out = outFragColor;
//}