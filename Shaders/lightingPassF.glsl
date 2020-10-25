#version 450 core
struct Light {
    vec3 position;
	vec3 color;
};

struct DirectionalLight
{
	vec3 direction;
	vec3 color;
	sampler2D shadowMap;
	mat4 lightSpaceMatrix;

};

struct PointLight
{
	vec3 position;
	vec3 color;
};

struct Spotlight
{
	vec3 position;
	vec3 direction;
	vec3 color;
	float cutoff; // maximum cos value of angle between light->frag vector and light direction 
	float outerCutoff;
};

const float gamma = 2.2;
const float dirLightPositionFactor = 10;


layout(location = 1) out vec4 brightOut;
layout(location = 0) out vec4 sceneOut;

in vec2 texCoords;

uniform vec3 viewPos;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec; 
//uniform sampler2D shadowMap;
uniform sampler2D reflectiveShadowMap;


uniform DirectionalLight dirLight0;

uniform float reflectiveSMsamples[800];

const float reflectFactor = 0.003;
const float rmax = 0.4;


float calcShadow(vec4 fragPosLightSpace, vec3 lightPos, vec3 normal);


float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec3 sampleFirstBounce(vec3 currPosition)
{
	vec4 lightSpacePos = dirLight0.lightSpaceMatrix*vec4(currPosition,1.0);
	vec2 texel = (lightSpacePos*0.5).rg + vec2(0.5);
	vec3 result = vec3(0);
	for(int i = 0; i<399; ++i)
	{
		float randomX = reflectiveSMsamples[i];
		float randomY = reflectiveSMsamples[i+1];
		
		vec2 texelToSample = vec2(texel.x + rmax*randomX*sin(6.28*randomY),texel.y+rmax*randomX*sin(6.28*randomY));	
		result += randomX*randomX*texture(reflectiveShadowMap,texelToSample).rgb;
	}

	return result*0.07;
}



void main()
{
	vec3 fragPos = texture(gPosition,texCoords).rgb;
	vec3 normal = texture(gNormal,texCoords).rgb;
	vec4 albedoSpec = texture(gAlbedoSpec,texCoords);
	vec3 viewDir = normalize(viewPos-fragPos);
	

	vec3 lightDir = normalize(dirLight0.direction);

	float lightDot = max(dot(normal,lightDir),0.0);
	vec3 lighting = albedoSpec.rgb * 0.1;  //hard coded ambient

	float shadow = calcShadow(dirLight0.lightSpaceMatrix*vec4(fragPos,1.0),dirLight0.direction * dirLightPositionFactor,normal);
	/*
	if(lightDot < 0.3)
	{
	lightDot = 0.2;
	}
	else if(lightDot < 0.925)
	{
	lightDot = 0.4;
	}
	else
	{
	lightDot = 1.0;
	}
	*/
		//diffuse
	vec3 diffuse = lightDot* albedoSpec.rgb * dirLight0.color * 0.65 *shadow;
	
	lighting += diffuse;
    vec3 specular = albedoSpec.a * albedoSpec.rgb * 1.0 * shadow;
	lighting += specular;

	sceneOut = vec4(lighting,1.0);

	//float brightness = dot(lighting.rgb, vec3(0.2126, 0.7152, 0.0722));
	float average = (lighting.r + lighting.g + lighting.b)/3;
	if(sceneOut.r > average || lighting.r == lighting.b)
		brightOut = sceneOut;
	else
		brightOut = vec4(0.0,0.0,0.0,1.0);

	brightOut = sceneOut;



}


float calcShadow(vec4 fragPosLightSpace, vec3 lightPos, vec3 normal)
{
    //this does nothing for orthographic matrices but is necessary for projection matrices
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w ;
    projCoords = projCoords * 0.5  + 0.5;
    if(projCoords.z > 1.0)
        return 1.0;
    float shadowBias = max(0.05 * (1.0 - dot(normal, lightPos)), 0.005); 
    float closestDepth = texture(dirLight0.shadowMap,projCoords.xy).r;
    float currentDepth = projCoords.z;

    vec2 texelSize = 1/textureSize(dirLight0.shadowMap,0);
    float shadow = 1.0;

    for(int x = -1;x<1;++x)
    {
        for(int y = -1;y<1;++y)
        {
            float pcfDepth = texture(dirLight0.shadowMap,projCoords.xy + vec2(x,y)*texelSize).r;
            shadow +=currentDepth - shadowBias > pcfDepth ? 0.0 : 1.0;
        }
    }
    //if curr depth > closestDepth, not in shadow , else it is in shadow
    //shadow = currentDepth - shadowBias > closestDepth ? 0.0: 1.0;

    shadow /= 9.0;

    return shadow;
}

float ambientLightRayTrace()
{

	return 1.0;
}