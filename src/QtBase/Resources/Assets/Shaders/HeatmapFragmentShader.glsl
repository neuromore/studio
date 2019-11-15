#version 120
varying vec4 colorV;

void main(void)
 {
     vec2 pos = mod(gl_FragCoord.xy, vec2(185.0)) - vec2(60.0);
     float dist_squared = dot(pos, pos);
  
	 if (dist_squared < 50.0)
	 {
		 gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
	 }
	 else if (50.0 < dist_squared && dist_squared < 900.0)
	 {
		     float frac = (dist_squared * 0.0011);
		     gl_FragColor = vec4(1.0, frac, frac, 1.0) * colorV;
	 }
	 else 
	 {
		gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
	 }
  }
