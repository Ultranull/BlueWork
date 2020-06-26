#version 430 core
out vec4 fragColor;

struct Material {
	sampler2D diffuse;
	vec4 specular;
	vec4 color;
	float shininess;
};

in vec3 normal;
in vec2 uv;
in vec3 FragPos;  

uniform Material material;


void main(){
	
    fragColor = material.color;
}

