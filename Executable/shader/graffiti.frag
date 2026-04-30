#version 460

in vec3 LightIntensity;
in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;
in vec3 Tangent;
in vec3 Bitangent;
in vec3 NormalInterp;
in vec3 Vec;

layout (location = 0) out vec4 FragColor;
layout (binding = 0) uniform sampler2D Tex1;

uniform struct SpotLightInfo{
    vec4 Position;
    vec3 La;
    vec3 L;
    vec3 Direction;
    float Exponent;
    float Cutoff;
}Spot;

uniform struct MaterialInfo{
    vec3 Kd;
    vec3 Ka;
    vec3 Ks;
    float Shininess;
}Material;

vec3 blinPhongSpotModel(vec3 position, vec3 n,vec3 texColour,MaterialInfo surface)
{
    float distance = length(Spot.Position.xyz - Position);
    float attenuation = 1.0 / (1.0 + 0.62 * distance + 0.5 * distance*distance);
   

    vec3 s =  normalize(Spot.Position.xyz - position);

    float cosAng = dot(-s,normalize(Spot.Direction));
    
    float angle = acos(cosAng);
    float sDotN = max(dot(s,n),0.0);

    float spotScale = 0.0;
    vec3 diffuse = vec3(0.0);
    vec3 spec = vec3(0.0);
    

    if(angle < Spot.Cutoff)
    {
        spotScale = pow(cosAng,Spot.Exponent);
        
        diffuse = surface.Kd  *sDotN * texColour * attenuation;
       
        if(sDotN > 0)
        {
            vec3 v = normalize(-position.xyz);
            vec3 h = normalize(s + v);
            spec = surface.Ks * pow(max(dot(n, h), 0.0), surface.Shininess) * attenuation;
        }
    }
    vec3 ambient = Spot.La * surface.Ka * texColour * attenuation;

    return ambient + Spot.L * (diffuse + spec);

}

void main() {
    
    float TextureSF = 3.5;
    float brightness = 3.0;

    vec3 n = normalize(Normal);

    vec4 graffitiTex = texture(Tex1, TexCoord * TextureSF);
    
    graffitiTex.rgb = pow(graffitiTex.rgb, vec3(0.45));

    vec3 lighting = vec3(0.0f);
    lighting += blinPhongSpotModel(Position, n, graffitiTex.rgb, Material);
    lighting *= brightness;

    float lightFactor = (lighting.r + lighting.g + lighting.b) / 3.0;
    lightFactor = clamp(lightFactor, 0.0, 1.0);
    
    FragColor = vec4(lighting, graffitiTex.a * lightFactor);
    //FragColor = vec4(vec3(cosAng),1);

    if(FragColor.a < 0.01)
    {
        discard;    
    }
    
}