#version 430 core
out vec4 fragColor;

struct Material {
	sampler2D diffuse;
	vec4 specular;
	vec4 color;
	float shininess;
};

in vec2 uv;

uniform Material material;


void main(){
	vec2 uvp = vec2(uv.x, 1-uv.y);
	fragColor = vec4(texture(material.diffuse,uvp).rgb, 1)+vec4(uv,0,0);
}

