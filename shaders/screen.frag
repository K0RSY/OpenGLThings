#version 330 core

in vec2 Tex;

out vec4 FragColor;

uniform sampler2D texture_data;

uniform float crt;

void main() {
    vec2 finalTex = Tex;
    
    float pi = 3.1415926;

    finalTex.y -= (Tex.y * 2 - 1) * (sin(Tex.x * pi) - 1.2) * crt;
    finalTex.x -= (Tex.x * 2 - 1) * (sin(Tex.y * pi) - 1.2) * crt;

    float lineTint = floor(mod((finalTex.y * 200), 2)) * .1 + .9;

    vec4 finalCol = texture(texture_data, finalTex) * lineTint;

    FragColor = finalCol;
}