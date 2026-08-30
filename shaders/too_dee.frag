#version 330 core

in vec2 Tex;
in float Zbf;

out vec4 FragColor;

uniform sampler2D texture_data;

void main() {
    vec2 finalTex = Tex;
    
    vec4 finalCol = texture(texture_data, finalTex);

    finalCol.a *= min(((Zbf) * 3), 1);
    
    FragColor = finalCol;
}