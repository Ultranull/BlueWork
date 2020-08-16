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
	float FarPlane;
	int shadowId;
	int shadow;
	float p;
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

uniform samplerCube depthmaps[10];

mat3 calcTBN(vec3 normal){
	mat3 MVP=mat3(view*model);
	mat3 normalMatrix=transpose(inverse(mat3(model)));
	vec3 T = normalize(normalMatrix * tangent);
	vec3 N = normalize(normalMatrix * normal);
	T = normalize(T - dot(T,N) * N);
	vec3 B = normalize(cross(T,N));
	return transpose(mat3(T,B,N));
}

vec4 calcLight(Point light, samplerCube shadowmap,vec3 normal, mat3 TBN){
	
	float shadow = 1.0;
	if(light.shadow==1){
		vec3 fragToLight = FragPos - light.position.xyz;
		float currentDepth = length(fragToLight);
		float bias = 0.25; 
		float counter=0;
		float size = 1., stp =size/2.;
		float fuzz = 20.;
		for(float x = -size; x <= size; x+=stp){
			for(float y = -size; y <= size; y+=stp){
				for(float z = -size; z <= size; z+=stp){
					float pcfDepth = texture(shadowmap, fragToLight + vec3(x, y, z)/fuzz ).r * 100.0; 
					shadow += currentDepth - bias > pcfDepth ? .0 : 1.0;    
					counter+=1.;
				}
			}    
		}
		shadow /= counter;
	}
	float visibility = clamp(shadow,0,1);

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
        
    return vec4(ambient + (diffuse + specular)*(attun*visibility),1);
}

void main(){
	
	vec4 result=vec4(0);
	vec3 mapnormal = normalize((texture(material.normal,uv).rgb*2.)-1.);
	mat3 TBN = calcTBN(normal);
	for(int i=0;i<numLights;i++)
		result+=calcLight(lights[i], depthmaps[lights[i].shadowId],mapnormal,TBN);
    fragColor = result;
}

