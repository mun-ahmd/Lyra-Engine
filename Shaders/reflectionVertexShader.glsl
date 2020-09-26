#version 450 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;

out vec3 position;
out vec3 normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	normal = normalize(mat3(transpose(inverse(model))) * aNormal);
	position = vec3(projection*view * model * vec4(aPos,1.0));
	gl_Position = vec4(position,1.0);
}