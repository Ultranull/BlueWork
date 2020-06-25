#version 430 core
precision highp float;

layout(location = 0) in vec3 vertpos;
layout(location = 2) in vec2 vertuv;
layout(location = 3) in vec3 vertnormal;

layout(location = 6) in vec3 offsets;

out vec3 FragPos;
out vec3 normal;
out vec2 uv;

layout(std140,binding=0)uniform camera{
	mat4 projection;
	mat4 view;
	vec4 up;
	vec4 direction;
	vec4 position;
	vec4 dummy;

};

uniform mat4 model;

void main(){
	vec4 pos=model*vec4(vertpos+offsets,1);
    gl_Position=projection*view*pos;
	FragPos=vec3(pos);
	normal=mat3(transpose(inverse(model))) * vertnormal;
	uv=vertuv;
}