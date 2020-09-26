#version 450 core

out vec4 frag;
in vec2 texCoords;
in float depth;

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_diffuse2;
};

const float reflectFactor = 0.3;

uniform Material material;

void main(){
	//this happens behind the scene anyways so I can just not write it
	//gl_FragDepth = gl_FragCoord.z;
	frag.rgb = texture(material.texture_diffuse1,texCoords).rgb * reflectFactor;
	frag.a = 1.0;
};