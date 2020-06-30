#version 330 core

layout(location = 0) in vec3 vertpos;
layout(location = 1) in vec2 vertuv;

out vec2 uv;

uniform mat4 model;

void main(){
	gl_Position=model*vec4(vertpos,1);
	uv=vertuv;
}

