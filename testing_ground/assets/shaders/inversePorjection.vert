#version 430 core
precision highp float;

layout(location = 0) in vec3 vertpos;
layout(location = 1) in vec2 vertuv;
layout(location = 2) in vec3 vertnormal;
layout(location = 3) in vec3 verttangent;

out vec4 screenSpacePos;
out vec3 FragPos;
out vec3 normal;
out vec2 uv;
out vec3 tangent;

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
	vec4 pos=model*vec4(vertpos,1);
	screenSpacePos=projection*view*pos;

	vec2 scaledUv = (vertuv*2 - 1);
	gl_Position = vec4(scaledUv,0,1);

	FragPos=vec3(pos);
	normal=vertnormal;
	uv=vertuv;
	tangent = verttangent;
}