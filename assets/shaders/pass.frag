#version 330 core
out vec4 fragcolor;

in vec3 color;
in vec2 uv;
uniform sampler2D passthrough;

void main(){
	vec2 raster=vec2(1,1)*64*100;
	vec2 id=fract(floor(uv*raster)/raster);

	vec3 col=texture(passthrough,id).xyz;

	fragcolor=vec4(col,1);
}