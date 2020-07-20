#version 450 core
out vec4 FragColor;

uniform Material {
    vec3 color_diffuse;
	vec3 color_specular;
	vec3 color_ambient;
	vec3 color_transparent;
	vec3 color_emissive;

	float shine;
	float ior;
	float opacity;
} material;

uniform sampler2D tx_diffuse;
uniform sampler2D tx_specular;

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
	vec3 lighting = vec3(0,0,0);

	for (int i = 0; i < dirLight_AMT; i++) {
		lighting += CalcDirLight(dirLights[i], norm, ViewDir);
	}
	for (int i = 0; i < pointLight_AMT; i++) {
//		lighting += CalcPointLight(pointLights[i], norm, FragPos, ViewDir);
	}
	for (int i = 0; i < spotLight_AMT; i++) {
//		lighting += CalcSpotLight(spotLights[i], norm, FragPos, ViewDir);
	}

	vec4 texColor = pow((vec4(lighting, 1.0)), vec4(1.0 / gamma));

	// vec3 reflection = CalcReflection(norm, FragPos, ViewPos, material.shine);
	// vec3 refraction = CalcRefraction(norm, FragPos, ViewPos, material.ior);

	// FragColor = vec4(vec3(texture(tx_specular, TexCoords)), 1);
    FragColor.rgb = texColor.rgb * texColor.a;
	// if(dirLight_AMT == 1) {
	// 	FragColor.rgb = dirLights[0].diffuse;
	// } else {
	// 	FragColor.rgb = vec3(1,1,1);
	// }
}

// calculates the color when using a directional light.
vec3 CalcDirLight(dirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    // vec3 halfwayDir = normalize(lightDir + viewDir);
	vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(tx_diffuse, TexCoords));
    // vec3 diffuse = light.diffuse * diff * mix(vec3(texture(tx_diffuse, TexCoords)), material.color_diffuse, .5);
	vec3 diffuse = light.diffuse * diff * mix(vec3(texture(tx_diffuse, TexCoords)), material.color_diffuse, .5);
    vec3 specular = light.specular * spec * mix(vec3(texture(tx_specular, TexCoords)), material.color_specular, .5);

    // return vec3(texture(tx_diffuse, TexCoords));
	return ambient + diffuse + specular;
}

// calculates the color when using a point light.
vec3 CalcPointLight(pointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient + material.color_ambient;
    vec3 diffuse = light.diffuse * diff * vec3(texture(tx_diffuse, TexCoords)) * material.color_diffuse;
    vec3 specular = light.specular * spec * vec3(texture(tx_specular, TexCoords)) * material.color_specular;
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
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shine);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * (vec3(texture(tx_diffuse, TexCoords)) + material.color_ambient);
    vec3 diffuse = light.diffuse * diff * (vec3(texture(tx_diffuse, TexCoords)) + material.color_diffuse);
    vec3 specular = light.specular * spec * (vec3(texture(tx_specular, TexCoords)) + material.color_specular);
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
    vec3 R = refract(I, normal, ior);
    return vec3(texture(skybox, R).rgb);
}

