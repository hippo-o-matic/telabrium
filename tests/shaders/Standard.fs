#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
	sampler2D reflective;

    vec3 diffuse_color;
    vec3 specular_color;
    vec3 ambient_color;

    float opacity;
    float shininess;
	float IOR;
}; 

struct dirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct pointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct spotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 ViewPos;

//Settings
#define MAX_DIRLIGHT 10
#define MAX_POINTLIGHT 10
#define MAX_SPOTLIGHT 10
float gamma = 2.2;

uniform int dirLight_AMT = 0;
uniform int pointLight_AMT = 0;
uniform int spotLight_AMT = 0;

uniform dirLight dirLights[MAX_DIRLIGHT];
uniform pointLight pointLights[MAX_POINTLIGHT];
uniform spotLight spotLights[MAX_SPOTLIGHT];

uniform Material mat;

uniform sampler2D texture_diffuse;
uniform samplerCube skybox;

// function prototypes
vec3 CalcDirLight(dirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(pointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(spotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

vec3 CalcReflection(vec3 normal, vec3 frag, vec3 view, float shine);
vec3 CalcRefraction(vec3 normal, vec3 frag, vec3 view, float ior);

void main() {
    // properties
    vec3 norm = normalize(Normal);
    vec3 ViewDir = normalize(ViewPos - FragPos);
	vec3 lighting;

	for (int i = 0; i < dirLight_AMT; i++) {
		lighting += CalcDirLight(dirLights[i], norm, ViewDir);
	}
	for (int i = 0; i < pointLight_AMT; i++) {
//		lighting += CalcPointLight(pointLights[i], norm, FragPos, ViewDir);
	}
	for (int i = 0; i < spotLight_AMT; i++) {
//		lighting += CalcSpotLight(spotLights[i], norm, FragPos, ViewDir);
	}

	vec4 texColor = vec4(pow((vec4(lighting, 1.0)).rgba, vec4(1.0 / gamma)));

//	vec3 reflection = CalcReflection(norm, FragPos, ViewPos, mat.shininess);
//	vec3 refraction = CalcRefraction(norm, FragPos, ViewPos, mat.IOR);

	FragColor = vec4(0,0,0,0);
//  FragColor.rgb = texColor.rgb * texColor.a + reflection.rgb;
}

// calculates the color when using a directional light.
vec3 CalcDirLight(dirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
    // combine results
    vec3 ambient = light.ambient * mat.ambient_color;
    vec3 diffuse = light.diffuse * diff * vec3(texture(mat.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(mat.specular, TexCoords));
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(pointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), mat.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * (vec3(texture(mat.diffuse, TexCoords)) + mat.ambient_color);
    vec3 diffuse = light.diffuse * diff * (vec3(texture(mat.diffuse, TexCoords)) + mat.diffuse_color);
    vec3 specular = light.specular * spec * (vec3(texture(mat.specular, TexCoords)) + mat.specular_color);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(spotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), mat.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * (vec3(texture(mat.diffuse, TexCoords)) + mat.ambient_color);
    vec3 diffuse = light.diffuse * diff * (vec3(texture(mat.diffuse, TexCoords)) + mat.diffuse_color);
    vec3 specular = light.specular * spec * (vec3(texture(mat.specular, TexCoords)) + mat.specular_color);
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}

vec3 CalcReflection(vec3 normal, vec3 frag, vec3 view, float shine) {
    vec3 I = normalize(frag - view);
    vec3 R = reflect(I, normal);
    return vec3(texture(skybox, R).rgb);
}

vec3 CalcRefraction(vec3 normal, vec3 frag, vec3 view, float ior){
    vec3 I = normalize(frag - view);
    vec3 R = refract(I, normal, 1/1.52 );
    return vec3(texture(skybox, R).rgb);
}

