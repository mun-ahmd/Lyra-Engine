//Forward Fragment Shader
#version 450 core
struct Material
{
	sampler2D texture_diffuse1; 
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    sampler2D texture_diffuse4;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
    sampler2D texture_specular3;
    sampler2D texture_specular4;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float shininess;
};

struct Light {
    vec3 position;
  
};

struct DirectionalLight
{
	vec3 direction;
    vec3 color;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight
{
	vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct Spotlight
{
	vec3 position;
	vec3 direction;
	vec3 color;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float cutoff; // maximum cos value of angle between light->frag vector and light direction 
	float outerCutoff;

    float constant;
    float linear;
    float quadratic;
};

out vec4 fragColor;

in vec3 normal;
in vec3 fragPos;
in vec2 texCoords;

uniform Material material;
uniform DirectionalLight dirLight0;
uniform PointLight ptLight;

uniform sampler2D shadowMap;

uniform vec3 viewPos;

vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec4 fragPosLightSpace);
vec3 calcDirectionalLightWithoutShadows(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calcSpotLight(Spotlight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float LinearizeDepth(float depth);
vec3 desaturate(vec3 originalColor, float desaturationFactor);
vec3 rgb2hsv(vec3 c);
vec3 hsv2rgb(vec3 c);
float calcShadow(vec4 fragPosLightSpace , vec3 lightPos);



float near = 0.1; 
float far  = 3000.0; 



void main()
{
    //fragColor = vec4(vec3(texture(shadowMap,texCoords).r),1.0);

    
	vec3 viewDir = normalize(viewPos - fragPos);
    //vec3 result = vec3(texture(material.texture_diffuse1,texCoords));
    //vec3 result = calcDirectionalLight(dirLight,normal,viewDir, fragPosLightSpace);
    vec3 result  = calcDirectionalLightWithoutShadows(dirLight0,normal,viewDir);

    fragColor = vec4(result,1.0);
    
    
}



vec3 rgb2hsv(vec3 c)
{
    //Understand how this works..Soon 
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}


vec3 hsv2rgb(vec3 c)
{
    //Understand how this works..Soon 
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}




float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

vec3 desaturate(vec3 originalColor, float desaturationFactor)   //takes in hsv values
{
/*
    //the commented region is if you want to use with rgb values
    //Understand how this algorithm works..Soon.
    //float f = 0.2; // desaturate by 20%
    float L = 0.3*originalColor.r + 0.6*originalColor.g + 0.1*originalColor.b;
    float new_r = originalColor.r + desaturationFactor* (L - originalColor.r);
    float new_g = originalColor.g + desaturationFactor* (L - originalColor.g);
    float new_b = originalColor.b + desaturationFactor* (L - originalColor.b);   
*/
    return vec3(originalColor.r,originalColor.g-desaturationFactor,originalColor.b);
}


float calcShadow(vec4 fragPosLightSpace, vec3 lightPos)
{
    //this does nothing for orthographic matrices but is necessary for projection matrices
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w ;
    projCoords = projCoords * 0.5  + 0.5;
    if(projCoords.z > 1.0)
        return 1.0;
    float shadowBias = max(0.05 * (1.0 - dot(normal, lightPos)), 0.005); 
    float closestDepth = texture(shadowMap,projCoords.xy).r;
    float currentDepth = projCoords.z;

    vec2 texelSize = 1/textureSize(shadowMap,0);
    float shadow = 0;

    for(int x = -1;x<1;++x)
    {
        for(int y = -1;y<1;++y)
        {
            float pcfDepth = texture(shadowMap,projCoords.xy + vec2(x,y)*texelSize).r;
            shadow +=currentDepth - shadowBias > pcfDepth ? 0.0 : 1.0;
        }
    }
    //if curr depth > closestDepth, not in shadow , else it is in shadow
    //shadow = currentDepth - shadowBias > closestDepth ? 0.0: 1.0;

    shadow /= 9.0;

    return shadow;
}


//Now uses blinn-phong model instead of phong model

// calculates the color when using a directional light.
vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec4 fragPosLightSpace)
{
    float shadow = calcShadow(fragPosLightSpace, light.direction * 100.0);

    vec3 lightDir = normalize(light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    //vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfWay = normalize(lightDir+viewDir);
    float spec = pow(max(dot(normal, halfWay), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, texCoords)) * shadow;
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, texCoords)) * shadow;
    return (ambient + diffuse + specular);
}

vec3 calcDirectionalLightWithoutShadows(DirectionalLight light, vec3 normal, vec3 viewDir)
{

    vec3 lightDir = normalize(light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    //vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfWay = normalize(lightDir+viewDir);
    float lightDot = max(dot(normal,lightDir),0.0);
    float spec = pow(max(dot(normal, halfWay), 0.0), material.shininess);
    vec4 albedoSpec = vec4(texture(material.texture_diffuse1,texCoords).rgb,texture(material.texture_specular1,texCoords).r);
    // combine results
    vec3 ambient = 0.2 * albedoSpec.rgb;
    vec3 diffuse = lightDot * light.color * 0.65 * albedoSpec.rgb ;
    vec3 specular = albedoSpec.rgb * albedoSpec.a * 1.0;
    return (ambient + diffuse +specular);
}



// calculates the color when using a point light.
vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir , vec3 fragPos, float shadow)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    //vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfWay = normalize(lightDir+viewDir);
    float spec = pow(max(dot(normal, halfWay), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    //float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, texCoords));

    ambient *= attenuation;
    diffuse *= attenuation * shadow;
    specular *= attenuation * shadow;
    
    if(light.position.x == 1)
    {
        return vec3(0,1,1);
    }
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 calcSpotLight(Spotlight light, vec3 normal, vec3 fragPos, vec3 viewDir, float shadow)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    //vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfWay = normalize(lightDir+viewDir);
    float spec = pow(max(dot(normal, halfWay), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, texCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity * shadow;
    specular *= attenuation * intensity * shadow;
    return (ambient + diffuse + specular);
}
