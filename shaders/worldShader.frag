#version 410 core

in vec2 TexCoord;
in vec3 fragPos;
in vec4 Normal;
in vec4 fragPosLightSpace;
in vec4 fragmentPosEyeSpace;

out vec4 fragmentColour;

uniform sampler2D diffuseTexture;
uniform int objectType;
uniform int activeFog;
 
vec3 dirLightDirection=vec3(0.0f,1.0f,1.0f);
 
#define NR_POINT_LIGHTS 54 
uniform vec3 pointLightsPosition[NR_POINT_LIGHTS];

//lighting
vec3 lightColor=vec3(1.0f, 1.0f, 1.0f);
float ambientStrength = 0.4f;

float constant = 1.0f;
float linear = 0.09f;
float quadratic = 0.032f;

uniform sampler2D shadowMap;

float computeShadow(){
	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	normalizedCoords = normalizedCoords * 0.5 + 0.5;
	if (normalizedCoords.z > 1.0f)
		return 0.0f;
	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
	float currentDepth = normalizedCoords.z;
	float bias=0.005f;
	float shadow = currentDepth-bias > closestDepth ? 1.0 : 0.0;
	return shadow;
}
float computeFog()
{
 float fogDensity = 0.05f;
 float fragmentDistance = length(fragmentPosEyeSpace);
 float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

 return clamp(fogFactor, 0.0f, 1.0f);
}



vec3 CalcDirLight()
{
    float shadow=computeShadow();
    vec3 normal = normalize(vec3(Normal));
    vec3 lightDir = normalize(dirLightDirection);//posibil trebuie modificat
    
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
    // combine results
    vec3 ambient  = ambientStrength * lightColor * vec3(texture(diffuseTexture, TexCoord));
    vec3 diffuse  = lightColor  * diff * vec3(texture(diffuseTexture, TexCoord));
    return (ambient + (1.0f-shadow)*diffuse);
}  

vec3 CalcPointLight(vec3 lightPosition)
{
    vec3 lightDir = normalize(lightPosition - fragPos);
    vec3 normal = normalize(vec3(Normal));

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // attenuation
    float distance    = length(lightPosition - fragPos);
    float attenuation = 1.0 /(constant + linear * distance + 
  			     quadratic * (distance * distance));    
    // combine results
    //vec3 ambient  =0.01f *lightColor  * vec3(texture(diffuseTexture, TexCoord));
    vec3 diffuse  = lightColor  *diff* vec3(texture(diffuseTexture, TexCoord));

    //ambient  *= attenuation;
    diffuse  *= attenuation;
    return (diffuse);
}
void main() {
    //vec3 result=vec3(0.0f);
    vec3 result = CalcDirLight();
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLightsPosition[i]);
    vec4 colorFromTexture=texture(diffuseTexture, TexCoord);
    if(objectType==3 && colorFromTexture.a<0.1){
        discard;
    }
    if(objectType==4){
        
    }
    if(activeFog==0){
      fragmentColour =vec4(min(result, 1.0f),1.0f);  
    }
    else{
       vec4 color =vec4(min(result, 1.0f),1.0f);
       float fogFactor = computeFog();
       vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
       fragmentColour = mix(fogColor, color, fogFactor);   
    }
}