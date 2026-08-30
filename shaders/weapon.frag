#version 330 core

in vec2 Tex;

out vec4 FragColor;

uniform sampler2D texture_data;

void main() {
    vec2 finalTex = Tex;
    
    vec4 finalCol = texture(texture_data, finalTex);

    if (finalCol.a < 0.1) {
        discard;
    }
    
    FragColor = finalCol;
}