#version 410

layout (location = 0) in vec3 Position;
layout (location = 2) in vec2 VertexTexCoord;

out vec2 TexCoord;

void main()
{
    TexCoordOut = TexCoord;
    gl_Position = vec4(Position.xy, 0.0, 1.0);
}