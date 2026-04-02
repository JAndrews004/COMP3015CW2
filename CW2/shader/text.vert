#version 460

layout (location = 0) in vec3 vertex; // x, y, z (z can be 0)
layout (location = 1) in vec2 texCoords;

uniform mat4 MVP;

out vec2 TexCoords;

void main()
{
    gl_Position = MVP * vec4(vertex, 1.0);
    TexCoords = texCoords;
}