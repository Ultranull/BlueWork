#version 430 core
out vec4 fragColor;

struct Material {
	sampler2D diffuse;
	sampler2D normal;
	sampler2D specularMap;
	vec4 specular;
	vec4 color;
	float shininess;
};

layout(std140,binding=0)uniform camera{
	mat4 projection;
	mat4 view;
	vec4 up;
	vec4 direction;
	vec4 position;
	vec4 dummy;

};

in vec4 screenSpacePos;
in vec3 normal;
in vec2 uv;
in vec3 FragPos;  
in vec3 tangent;  

uniform Material material;
uniform mat4 model;

void main(){
	
	vec4 result=vec4(0);

	vec4 viewDir = position - vec4(FragPos,0);
	vec3 correctedNorm = normalize(transpose(inverse(mat3(model)))*normal);
	float frontFacing = dot(correctedNorm, viewDir.xyz);
	vec2 scaledScreenUv = (screenSpacePos.xy+1)/2.;

	if((scaledScreenUv.x > 1 || scaledScreenUv.y > 1) ||
	   (scaledScreenUv.x < 0 || scaledScreenUv.y < 0) ||
	   frontFacing<0)
		fragColor = vec4(1);//texture(material.diffuse,uv);
	else
		fragColor = texture(material.diffuse,scaledScreenUv);

}