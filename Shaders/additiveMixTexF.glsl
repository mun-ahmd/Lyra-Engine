#version 330 core
out vec4 FragColor;
  
in vec2 texCoords;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float exposure;

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(texture1, texCoords).rgb;      
    vec3 bloomColor = texture(texture2, texCoords).rgb;
    hdrColor += bloomColor; // additive blending
    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);
}  