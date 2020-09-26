#version 450 core

in vec3 position;
in vec3 normal;

out vec4 FragColor;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{
	vec3 vectorTowardsView = normalize(position - cameraPos);
	vec3 texCoordToBeShownByReflection = normalize(reflect(vectorTowardsView,normal));

	FragColor = vec4(texture(skybox,texCoordToBeShownByReflection).rgb , 1.0);
}