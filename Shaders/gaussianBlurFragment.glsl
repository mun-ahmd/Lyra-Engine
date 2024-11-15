#version 450 core
out vec4 fragColor;
  
in vec2 texCoords;

uniform sampler2D image;
  
uniform int horizontal; //1 if horizontal, 0 if vertical 
//for loop runs 5 * horizontal/vertical value, so if it is vertical horizontal loop runs 0 times 

uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{             
    vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
    vec3 result = texture(image, texCoords).rgb * weight[0]; // current fragment's contribution
    for(int i = 1; i < (5*horizontal); ++i)
    {
      result += texture(image, texCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
      result += texture(image, texCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
    }
    for(int i = 1; i < (5*(1-horizontal)); ++i)
    {
        result += texture(image, texCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        result += texture(image, texCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
    }
    fragColor = vec4(result, 1.0);
}