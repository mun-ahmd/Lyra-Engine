#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

out vec2 texCoords;
out float depth;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    texCoords = aTexCoords;
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
    depth = gl_Position.z;
}  