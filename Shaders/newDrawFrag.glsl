#version 450 core
out vec4 fragColor;

in vec2 texCoords;
in vec3 normal;

uniform sampler2D diffuse;
uniform sampler2D specular;
uniform sampler2D roughness;
uniform sampler2D metallic;
uniform sampler2D transparancy;


//for testing purpose
vec3 lightPos = vec3(1.0,0.5,0.3);
vec3 lightDir = vec3(1.0,0.3,0.2);

void main()
{
	vec3 color = texture(diffuse,texCoords).rgb;
	color = color * max(dot(lightDir,normal),0.0);
	fragColor = vec4(color,1.0);
	
}	