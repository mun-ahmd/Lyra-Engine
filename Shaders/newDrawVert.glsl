	//Light Vertex Shader
#version 450 core
out vec3 normal;
out vec3 fragPos;
out vec2 texCoords;

layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoords;

uniform mat4 model = mat4(1);
uniform mat4 view;
uniform mat4 projection;


void main()
{
	gl_Position = projection*view*model*vec4(aPos,1.0);
	//I should be doing the next line on cpu... Too bad!
	normal = normalize(transpose(inverse(mat3(model))) * aNormal);
	fragPos = vec3(model*vec4(aPos,1.0));
	texCoords = aTexCoords;
}