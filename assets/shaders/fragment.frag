#version 430 core
out vec4 fragColor;


struct Material {
	sampler2D diffuse;
	vec4 specular;
	vec4 color;
	float shininess;
};


struct Light{
	vec4 ambient;
	vec4 color;
	vec4 specular;
	vec4 attn;
	vec4 position;
};
layout(std140,binding=1)uniform Lights{
	Light lights[5];

};

layout(std140,binding=0)uniform camera{
	mat4 projection;
	mat4 view;
	vec4 up;
	vec4 direction;
	vec4 position;
	vec4 dummy;

};

in vec3 normal;
in vec2 uv;
in vec3 FragPos;  

uniform Material material;

vec4 calcLight(Light light){
	
	float dist=length(light.position.xyz-FragPos);
	float attun= 1./(1+light.attn.y*dist+light.attn.x*dist*dist);
    // ambient
    vec3 ambient = vec3(light.ambient * texture(material.diffuse,uv).rgb);
  	
    // diffuse 
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position.xyz - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (light.color * material.color) * (diff * texture(material.diffuse,uv).rgb);
    
    // specular
    vec3 viewDir = normalize(position.xyz - FragPos);
//    vec3 reflectDir = reflect(-lightDir, norm);
	vec3 reflectDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, reflectDir), 0.0), material.shininess);
    vec3 specular = (light.specular.xyz * material.specular) * (spec);  
        
    return vec4((ambient + diffuse + specular)*attun,1);
}

void main(){
	
	vec4 result=vec4(0);
	for(int i=0;i<5;i++)
		result+=calcLight(lights[i]);
    fragColor = result;
}

