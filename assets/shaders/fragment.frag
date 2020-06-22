#version 430 core
out vec4 fragColor;

struct Material {
	sampler2D diffuse;
	vec4 specular;
	vec4 color;
	float shininess;
};


struct Point{
	vec4 ambient;
	vec4 color;
	vec4 specular;
	vec4 attn;
	vec4 position;
};

struct Directional {
	vec4 ambient;
	vec4 color;
	vec4 specular;
	vec4 att;
	vec4 direction;
};

#define maxNumLights 50
uniform uint numLights;


layout(std140,binding=1)uniform Lights{
	Point lights[maxNumLights];
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

vec3 calcDirLight(Directional light)
{
    vec4 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir.xyz), 0.0);
    //vec4 reflectDir = reflect(-lightDir, vec4(normal,0));
	vec4 reflectDir = normalize(lightDir + direction);
    float spec = pow(max(dot(direction, reflectDir), 0.0), material.shininess);

    vec3 ambient = vec3(light.ambient.xyz * texture(material.diffuse,uv).rgb);
    vec3 diffuse = (light.color.xyz * material.color.xyz) * (diff * texture(material.diffuse,uv).rgb);
    vec3 specular = (light.specular.xyz * material.specular.xyz) * (spec);  
    return (ambient + diffuse + specular);
}  

vec4 calcLight(Point light){
	
	float dist=length(light.position.xyz-FragPos);
	float attun= light.attn.w/(light.attn.z+light.attn.y*dist+light.attn.x*dist*dist);
    // ambient
    vec3 ambient = vec3(light.ambient.xyz * texture(material.diffuse,uv).rgb)* material.color.xyz;
  	
    // diffuse 
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position.xyz - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (light.color.xyz * material.color.xyz) * (diff * texture(material.diffuse,uv).rgb);
    
    // specular
    vec3 viewDir = normalize(position.xyz - FragPos);
    vec3 reflectDir = reflect(norm, lightDir);
	//vec3 reflectDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
    vec3 specular = (light.specular.xyz * material.specular.xyz) * (spec);  
        
    return vec4((ambient + diffuse + specular)*attun,1);
}

void main(){
	
	vec4 result=vec4(0);
	for(int i=0;i<numLights;i++)
		result+=calcLight(lights[i]);
    fragColor = result;
}

