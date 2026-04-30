#version 460

layout (location = 0) in vec3 VertexPosition;

out vec3 Vec;


uniform mat4 ModelViewMatrix;
uniform mat4 MVP;
uniform mat3 NormalMatrix;

void main()
{
    Vec = VertexPosition;
    gl_Position = MVP * vec4(VertexPosition, 1.0);
}