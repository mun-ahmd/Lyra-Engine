#version 450 core
out vec4 fragColor;
  
in vec3 texCoords;

uniform vec3 bottomColor = vec3(0.85,180/256,0.14);
uniform vec3 topColor = vec3(0.125,0.251,0.6);

float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
float offset = 0.05;
float offsets[5] = float[] (0.00,offset ,-offset,offset/2,-offset/2);

void main()
{
    const float gamma = 2.2;
    const float exposure = 1.1;
    vec3 hdrColor = vec3(0.0);
    for(int i = 0; i < 5 ; ++i)
    {
    float yForLerp = clamp(texCoords.g + offsets[i],0.15,0.95);
    hdrColor += mix(bottomColor,topColor,yForLerp) * weight[0];
    }

    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));
    
    fragColor = vec4(result, 1.0);
}