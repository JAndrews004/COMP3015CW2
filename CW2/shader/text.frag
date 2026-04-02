#version 460
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D text; // your glyph texture
uniform vec3 textColor; // <-- add this

void main()
{
    float alpha = texture(text, TexCoords).r;
    FragColor = vec4(textColor, alpha);
}