#version 410


in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

layout(location = 0) out vec4 FragColor;






void main()
{
    if(Pass ==1){
        pass1();
    }
    else if(Pass==2){
        pass2();
    }
}

