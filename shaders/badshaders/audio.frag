void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // create pixel coordinates
	vec2 uv = fragCoord.xy / iResolution.xy;
	vec2 mv = iMouse.xy / iResolution.xy;

//	// first texture row is frequency data
//	float fft  = texture2D( iChannel0, vec2(uv.x,0.25) ).x;
//
//    // second texture row is the sound wave
//	float wave = texture2D( iChannel0, vec2(uv.x,0.75) ).x;
//
//	// convert frequency to colors
//	vec3 col = vec3(fft, fft, fft);
//
//    // add wave form on top
//	//col += 1.0 -  smoothstep( 0.0, 0.01, abs(wave - uv.y) );
//
//    //col = pow( col, vec3(1.0,0.5,2.0) );
//
//	// output final color
//	fragColor = vec4(col,1.0);

    //cogl_tex_coord
//    vec2 tv = uv.xy / cogl_tex_coord.xy;

    // Output a random color:
//    fragColor = vec4(0.0, 1.0, 0.5, 1.0);

    // Output the actors content color:
    fragColor = texture2D( cogl_sampler, vec2(uv.x,uv.y));
}