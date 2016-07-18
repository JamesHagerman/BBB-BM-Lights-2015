Intro to Fragment Shaders
=========================

A fragment shader is nothing more than a function that calculates a single color.

Seriously, that's it. That's all it does.

You run it, and it says "Blue!" or "Red!" or "Transparent Yellow!"


How the hell is a shader useful?
================================

The trick with shaders is you don't just run it once... because that would only give you one color!

Shaders run once for EVERY PIXEL on the screen. In Shader World, pixels are called "fragments"... but that name doesn't matter to us.


But if we run the same function for ever pixel, won't the whole screen be the same color?
===============================

Not necessarily.

The shader KNOWS what pixel it's calculating a color for! The shader can use the pixels specific screen coordinate to do some pretty complex things!


Most basic shader
=================

Now that the basic theory is out of the way, let's show you the most basic shader possible!

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}

That's it! In this case, the color it's calculating is RED... so the WHOLE SCREEN will be RED when this shader is run.

Now, this is where I might lose some people: "fragColor" is the shader's OUTPUT color. EVERY shader has to, at some point, have a line that looks something like:

fragColor = somecolor;


About Colors
============

Shaders think about colors in a pretty obvious way. They are basically 4 values; red, blue, green, and transparent (also known as alpha)

The shader will mix these 4 values into a single color that will be displayed on the screen.

Shaders have a special way of representing these 4 colors: a 4th dimensional VECTOR!...otherwise known as a "vec4".

To build a vec4, you do something like we say in that simple shader example:

vec4(red_value, green_value, blue_value, alpha_value)

Keep in mind, that these values have a range between 0.0 and 1.0!!!!
All vectors (vec2, vec3, vec4) MUST have a decimal point in their values!


Back to the simple example
==========================

Okay! So, now, you should be able to understand that simple example!

But that's not really enough to do anything cool quite yet...


Shaders that use Pixel coordinates
===========================

The very next large trick to do with shaders is using the pixels coordinates to do special tricks!

The pixels coordinate is called "fragCoord"... it's ANOTHER VECTOR!! This time, it's a 2 dimensional vector, a vec2!

One values is the width, one value is the height.

0,0 is the upper left hand corner of the screen
max_window_width, max_window_height is the LOWER RIGHT corner of the screen!!!

Because the width and height are not between 0.0 and 1.0, we can not directly use the pixels coordinate (fragCoord) to determine color values (because they go between 0.0 and 1.0)!

However, we can divide fragCoord.xy by iResolution.xy (the actual resolution of the screen) to FORCE THE VALUES INTO THE 0.0 to 1.0 range!

In the shader below, we do that calculation, and then store the new value in a new vec2 named "uv".

The vec2 named uv will be the root of a LOT of our shaders

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 uv = fragCoord.xy / iResolution.xy;
	fragColor = vec4(uv.x, uv.y, 0.0, 1.0);
}

What will THIS shader put on the screen???



Useful GLSL functions
=====================


Smooth a value into a range:
smoothstep(lower, upper, input); // https://www.opengl.org/sdk/docs/man/html/smoothstep.xhtml:





GLSL Helper functions
=====================

Convert HSV to RGB
-------------------

vec3 hsv2rgb( in vec3 c )
{
    vec3 rgb = clamp( abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0, 0.0, 1.0 );
	return c.z * mix( vec3(1.0), rgb, c.y);
}

Smooth HSV to RGB conversion
-------------------

vec3 hsv2rgb_smooth( in vec3 c )
{
    vec3 rgb = clamp( abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0, 0.0, 1.0 );
	rgb = rgb*rgb*(3.0-2.0*rgb); // cubic smoothing	
	return c.z * mix( vec3(1.0), rgb, c.y);
}


Mirror over horizon
--------------------




















