#version 430 core
precision highp float;

layout(location = 0) in vec3 vertpos;
layout(location = 1) in vec2 vertuv;
layout(location = 2) in vec3 vertnormal;

out vec4 fragcolor;
out vec3 fragcoord;
out vec3 fragnormal;

uniform mat4 projection;
uniform mat4 view;
uniform float time;

float random(vec2 p){
	return fract(sin(dot(p,vec2(15.24523,63.23421)))*123128.223423);
}

float noise(vec2 p){
	vec2 i=floor(p),
         f=fract(p);
    
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));
    vec2 u = f*f*(3.0-2.0*f);
    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}
float cloud(vec2 uv,int layers){
    float p=2.,h=.5,o=0.;
    for(int i=0;i<layers;i++){
    	o+=noise(uv*p)*h;
        p*=2.;
        h/=2.;
    }
    return o;
}

void main(){
    float col;
    col+=(cloud(vertpos.xz,50))+.2;
	col-=length(vertpos.xz)*.1;
	if(col>.7)
		fragcolor=1.5*vec4(mix(vec3(0),mix(vec3(1),vec3(.1),.5),abs(col)),1.);
	else if(col<.55)
		fragcolor=1.5*vec4(mix(vec3(0),mix(vec3(.7,.7,0),vec3(.1),.5),abs(col)),1.);
	else fragcolor=1.5*vec4(mix(vec3(0),mix(vec3(.2,.7,0),vec3(.1),.5),abs(col)),1.);
//fragcolor=vertcolor;
    gl_Position=projection*view*vec4(vertpos+vec3(0,col,0),1);

	fragcoord=vertpos;
	fragnormal=vertnormal;
}