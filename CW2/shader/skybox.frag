#version 460

layout(binding=0) uniform samplerCube SkyBox;

uniform struct FogInfo{
    vec3 color;
    float density;
    float start;  
    float end;
    float enabled;   //1 = on, 0 = off
}Fog;


in vec3 Vec;

out vec4 FragColor;

void main() 
{
	vec3 texColor = texture(SkyBox,normalize(Vec)).rgb;

    vec3 finalColour;
    if (Fog.enabled > 0.5)
        finalColour = Fog.color;
    else
        finalColour = texColor;

    FragColor = vec4(finalColour, 1.0);

}