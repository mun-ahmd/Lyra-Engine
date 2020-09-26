//Geometry Pass Fragment Shader
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

//out vec4 fragColor;
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;


in vec3 normal;
in vec3 fragPos;
in vec2 texCoords;

uniform Material material;


void main()
{

    gPosition = fragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(normal);
    // and the diffuse per-fragment color
    gAlbedoSpec.rgb = texture(material.texture_diffuse1, texCoords).rgb;
    // store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = texture(material.texture_specular1, texCoords).r;
}
