#version 450 core 
out vec4 fragColor;

in vec2 texCoords;

uniform sampler2D texture1;

void main()
{
	vec4 result = texture(texture1,texCoords);
	fragColor = result;
}