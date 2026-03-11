#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;
layout (location = 3) in vec4 VertexTangent;

out vec3 Position;
out vec3 Normal;
out vec3 Tangent;
out vec3 Bitangent;
out vec3 NormalInterp;
out vec2 TexCoord;

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;
uniform mat3 NormalMatrix;



void main()
{
    vec3 N = normalize(NormalMatrix * VertexNormal);
    vec3 T = normalize(NormalMatrix * VertexTangent.xyz);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T) * VertexTangent.w;

    Tangent = T;
    Bitangent = B;
    NormalInterp = N;
    Normal = normalize(NormalMatrix*VertexNormal);
    Position = (ModelViewMatrix * vec4(VertexPosition,1.0)).xyz;
    TexCoord = VertexTexCoord;
    gl_Position = MVP * vec4(VertexPosition, 1.0);

}