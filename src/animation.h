#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include <clutter/clutter.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "p9813.h"
#include "TCLControl.h"

#ifndef TCL_ANIMATION
#define TCL_ANIMATION

class Animation {
public:
    Animation();

    Animation(ClutterActor *stage, TCLControl *tcl,
              ClutterActor *infoDisplay); //TCLControl tcl
    ~Animation();

    void switchAnimation(int animation_num, ClutterActor *infoDisplay);
    int getCurrentAnimation();

    // Timeline object itself:
    ClutterTimeline *timeline;
    TCLControl *tcl;

    int i;
    int currentAnimation;

    ClutterActor *rect;
};

//const gchar *fragShader = "" //"#version 110\n\n"
//        "uniform float iGlobalTime;\n"
//        "uniform vec2 iResolution;\n"
//        "uniform vec2 iMouse;\n"
//        "void mainImage(out vec4 fragColor, in vec2 fragCoord) {\n"
//        "   vec2 uv = fragCoord.xy / iResolution.xy;\n"
//        "   fragColor = vec4(uv.x, uv.y, 0.5+0.5*sin(iGlobalTime), 1.0);\n"
//        "}\n"
//        "void main(void) {\n"
//        "   vec4 outFragColor = vec4(1.0,0.5,0,0);\n"
//        "   vec2 inFragCoord = vec2(cogl_tex_coord_in[0].x*iResolution.x, cogl_tex_coord_in[0].y*iResolution.y);\n"
//        "   mainImage(outFragColor, inFragCoord);\n"
//        "   cogl_color_out = outFragColor;\n"
//        "}";

//const gchar *fragShader = "" //"#version 110\n\n"
//        "uniform float iGlobalTime;\n"
//        "uniform float width;\n"
//        "uniform float height;\n"
//        "void main(void) {\n"
//        "   vec2 res = vec2(50, 12);\n"
//        "   cogl_color_out = vec4(cogl_tex_coord_in[0].x+sin(iGlobalTime*0.05), cogl_tex_coord_in[0].y+cos(iGlobalTime*0.01), sin(iGlobalTime*cogl_tex_coord_in[0].x*500.0 + cogl_tex_coord_in[0].y), 1.0);\n"
//        "}";



const gchar *fragShader = "" //"#version 110\n\n"
        "uniform float iGlobalTime;\n"
        "uniform vec2 iResolution;\n"
        "uniform vec2 iMouse;\n"
        "float noise(vec3 p) //Thx to Las^Mercury\n"
        "{\n"
        "	vec3 i = floor(p);\n"
        "	vec4 a = dot(i, vec3(1., 57., 21.)) + vec4(0., 57., 21., 78.);\n"
        "	vec3 f = cos((p-i)*acos(-1.))*(-.5)+.5;\n"
        "	a = mix(sin(cos(a)*a),sin(cos(1.+a)*(1.+a)), f.x);\n"
        "	a.xy = mix(a.xz, a.yw, f.y);\n"
        "	return mix(a.x, a.y, f.z);\n"
        "}\n"
        "\n"
        "float sphere(vec3 p, vec4 spr)\n"
        "{\n"
        "	return length(spr.xyz-p) - spr.w;\n"
        "}\n"
        "\n"
        "float flame(vec3 p)\n"
        "{\n"
        "	float d = sphere(p*vec3(1.,.5,1.), vec4(.0,-1.,.0,1.));\n"
        "	return d + (noise(p+vec3(.0,iGlobalTime*2.,.0)) + noise(p*3.)*.5)*.25*(p.y) ;\n"
        "}\n"
        "\n"
        "float scene(vec3 p)\n"
        "{\n"
        "	return min(100.-length(p) , abs(flame(p)) );\n"
        "}\n"
        "\n"
        "vec4 raymarch(vec3 org, vec3 dir)\n"
        "{\n"
        "	float d = 0.0, glow = 0.0, eps = 0.02;\n"
        "	vec3  p = org;\n"
        "	bool glowed = false;\n"
        "\n"
        "	for(int i=0; i<64; i++)\n"
        "	{\n"
        "		d = scene(p) + eps;\n"
        "		p += d * dir;\n"
        "		if( d>eps )\n"
        "		{\n"
        "			if(flame(p) < .0)\n"
        "				glowed=true;\n"
        "			if(glowed)\n"
        "       			glow = float(i)/64.;\n"
        "		}\n"
        "	}\n"
        "	return vec4(p,glow);\n"
        "}\n"
        "\n"
        "void mainImage( out vec4 fragColor, in vec2 fragCoord )\n"
        "{\n"
        "	vec2 v = -1.0 + 2.0 * fragCoord.xy / iResolution.xy;\n"
        "	v.x *= iResolution.x/iResolution.y;\n"
        "\n"
        "	vec3 org = vec3(0., -2., 4.);\n"
        "	vec3 dir = normalize(vec3(v.x*1.6, -v.y, -1.5));\n"
        "\n"
        "	vec4 p = raymarch(org, dir);\n"
        "	float glow = p.w;\n"
        "\n"
        "	vec4 col = mix(vec4(1.,.5,.1,1.), vec4(0.1,.5,1.,1.), p.y*.02+.4);\n"
        "\n"
        "	fragColor = mix(vec4(0.), col, pow(glow*2.,4.));\n"
        "	//fragColor = mix(vec4(1.), mix(vec4(1.,.5,.1,1.),vec4(0.1,.5,1.,1.),p.y*.02+.4), pow(glow*2.,4.));\n"
        "\n"
        "}\n"
        "void main(void) {\n"
        "   vec4 outFragColor = vec4(1.0,0.5,0,0);\n"
        "   vec2 inFragCoord = vec2(cogl_tex_coord_in[0].x*iResolution.x, cogl_tex_coord_in[0].y*iResolution.y);\n"
        "   mainImage(outFragColor, inFragCoord);\n"
        "   cogl_color_out = outFragColor;\n"
        "}";







#endif












