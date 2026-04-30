#version 460

in vec3 LightIntensity;
in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;
in vec3 Tangent;
in vec3 Bitangent;
in vec3 NormalInterp;

in vec3 FragPosWorld;

layout (location = 0) out vec4 FragColor;
layout (binding = 0) uniform sampler2D Tex1;
layout (binding = 1) uniform sampler2D NormalMap;
layout (binding = 2) uniform sampler2D Tex2;
layout (binding = 3) uniform sampler2D puddleMask;
layout (binding = 4) uniform sampler2D shadowMap;

uniform mat4 LightSpaceMatrix;

uniform float normScale;

uniform struct LightInfo{
    vec4 Position;
    vec3 La;
    vec3 L;
   
}lights[4];

uniform struct MaterialInfo{
    vec3 Kd;
    vec3 Ka;
    vec3 Ks;
    float Shininess;
}Material;

uniform struct SpotLightInfo{
    vec4 Position;
    vec3 La;
    vec3 L;
    vec3 Direction;
    float Exponent;
    float Cutoff;
}Spot[2];

uniform struct FogInfo{
    vec3 color;
    float density;
    float start;  
    float end;
    float enabled;   //1 = on, 0 = off
}Fog;

const int levels = 4;
const float scaleFactor = 1.0/levels;


vec3 blinnPhongModel(int light, vec3 position,vec3 n,vec3 texColour,MaterialInfo surface)
{
    float distance = length(lights[light].Position.xyz - Position);
    float attenuation = 1.0 / (1.0 + 0.05 * distance + 0.08 * distance*distance);
    if(light==3){
     attenuation = 1.0;
     }
    vec3 ambient = lights[light].La * surface.Ka * texColour;

    vec3 s = normalize(vec3(lights[light].Position.xyz -position));
    float sDotN = max(dot(s,n),0.0);
    //vec3 diffuse = surface.Kd  * floor(sDotN*levels)*scaleFactor; // toon shading
    vec3 diffuse = surface.Kd  *sDotN * texColour * attenuation;

    vec3 spec = vec3(0.0);
    if(sDotN >0){
        vec3 v=normalize(-position.xyz);
        vec3 h = normalize(s + v);
        spec = surface.Ks * pow(max(dot(n, h), 0.0), surface.Shininess) * attenuation;
     }
     return ambient + lights[light].L * diffuse + spec * lights[light].L;
     
}
vec3 blinPhongSpotModel(int i,vec3 position, vec3 n,vec3 texColour,MaterialInfo surface)
{
    float distance = length(Spot[i].Position.xyz - Position);
    float attenuation = 1.0 / (1.0 + 0.12 * distance + 0.16 * distance*distance);

    vec3 s =  normalize(Spot[i].Position.xyz - position);

    float cosAng = dot(-s,normalize(Spot[i].Direction));
    
    float angle = acos(cosAng);
    float sDotN = max(dot(s,n),0.0);

    float spotScale = 0.0;
    vec3 diffuse = vec3(0.0);
    vec3 spec = vec3(0.0);
    

    if(angle < Spot[i].Cutoff)
    {
        spotScale = pow(cosAng,Spot[i].Exponent);
        
        diffuse = surface.Kd  *sDotN * texColour * attenuation;
       
        if(sDotN > 0)
        {
            vec3 v = normalize(-position.xyz);
            vec3 h = normalize(s + v);
            spec = surface.Ks * pow(max(dot(n, h), 0.0), surface.Shininess) * attenuation;
        }
    }
    vec3 ambient = Spot[i].La * surface.Ka * texColour * attenuation;

    return ambient + spotScale * Spot[i].L * (diffuse + spec);

}

float ShadowCalculation(vec3 fragPosWorld)
{
    vec4 fragPosLightSpace = LightSpaceMatrix * vec4(fragPosWorld, 1.0);

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0)
        return 0.0;

    float bias = 0.002;
    float shadow = 0.0;

    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += (projCoords.z > pcfDepth + bias) ? 1.0 : 0.0;
        }
    }

    shadow /= 9.0;

    return shadow;
}

void main() {
     mat3 TBN = mat3(
    normalize(Tangent),
    normalize(Bitangent),
    normalize(NormalInterp)
    );

    float TextureSF = 8.0;
    float NormalMapSF = 2.0;
    float MapSF = 2.0;
   

    vec3 lighting = vec3(0.0f);
    vec3 nMap = texture(NormalMap, TexCoord*normScale).rgb;

    nMap = normalize(nMap * 2.0 - 1.0); //Convert from [0,1] to [-1,1]
    vec3 n = normalize(TBN * nMap);
    //vec3 n = normalize(Normal);
    vec4 baseColour =  texture(Tex1,TexCoord*TextureSF);
    vec4 mossColour =  texture(Tex2,TexCoord*TextureSF);
    vec3 texColour = mix(baseColour.rgb,mossColour.rgb,mossColour.a);

    float wetness = texture(puddleMask, TexCoord*MapSF).r;
    MaterialInfo surface;

    surface.Kd = mix(Material.Kd, vec3(0.2),wetness);
    surface.Ka = mix(Material.Ka, vec3(0.1),wetness);
    surface.Ks = mix(Material.Ks, vec3(0.7),wetness);
    surface.Shininess = 128;

    vec3 darkGrey = vec3(0.75, 0.75, 0.75);
    texColour = mix(texColour,darkGrey,wetness);

    

    vec3 maskRGB = texture(puddleMask, TexCoord).rgb;

    float shadow = ShadowCalculation(FragPosWorld);

    for(int i =0;i<4;i++)
    {
        lighting += blinnPhongModel(i,Position,n,texColour,surface);
    }
    for(int i =0; i<2;i++){
        vec3 result = blinPhongSpotModel(i,Position,n,texColour,surface);
        if(i==1){
            result *= (1.0 - shadow);
        }
        lighting += result;
    }
    

    
    float fragDistance = abs(Position.z);

    float fogFactor = clamp((Fog.end - fragDistance) / (Fog.end - Fog.start), 0.0, 1.0);
    
    vec3 finalColour;
    if (Fog.enabled > 0.5)
        finalColour = mix(Fog.color, lighting, fogFactor);
    else
        finalColour = lighting;

    finalColour = pow(finalColour,vec3(0.45)); // gamma correction
    FragColor = vec4(finalColour, 1.0);
    
}