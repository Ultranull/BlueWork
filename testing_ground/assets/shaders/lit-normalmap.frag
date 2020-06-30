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
in vec3 tangent;  

uniform Material material;
uniform mat4 model;

mat3 calcTBN(vec3 normal){
	mat3 MVP=mat3(view*model);
	mat3 normalMatrix=transpose(inverse(mat3(model)));
	vec3 T = normalize(normalMatrix * tangent);
	vec3 N = normalize(normalMatrix * normal);
	T = normalize(T - dot(T,N) * N);
	vec3 B = normalize(cross(T,N));
	return transpose(mat3(T,B,N));
}

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

vec4 calcLight(Point light,vec3 normal, mat3 TBN){
	
	float dist=length(TBN*(light.position.xyz-FragPos));
	float attun= light.attn.w/(light.attn.z+light.attn.y*dist+light.attn.x*dist*dist);
    // ambient
    vec3 ambient = light.ambient.xyz * texture(material.diffuse,uv).rgb * material.color.xyz;
  	
    // diffuse 
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(TBN*(light.position.xyz - FragPos));
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (light.color.xyz * material.color.xyz) * (diff * texture(material.diffuse,uv).rgb);
    
    // specular
    vec3 viewDir = normalize(TBN*(position.xyz - FragPos));
    vec3 reflectDir = reflect(-lightDir, norm);
	//vec3 reflectDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = (light.specular.xyz * material.specular.xyz) * (spec* texture(material.specularMap,uv).rgb);  
        
    return vec4((ambient + diffuse + specular)*attun,1);
}

void main(){
	
	vec4 result=vec4(0);
	vec3 mapnormal = normalize((texture(material.normal,uv).rgb*2.)-1.);
	mat3 TBN = calcTBN(normal);
	for(int i=0;i<numLights;i++)
		result+=calcLight(lights[i],mapnormal,TBN);
    fragColor = result;
}

