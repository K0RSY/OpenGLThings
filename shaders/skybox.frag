#version 330 core

in vec2 Tex;

out vec4 FragColor;

uniform float time;

uniform sampler2D texture_data;
uniform sampler2D texture_datb;

void main() {
    vec2 finalTex = Tex;

    vec4 noise = texture(texture_datb, Tex);

    // finalTex = finalTex + sin(finalTex * 8) / (10 + ((sin(time / 100) + 1) / 2) * 50);
    // finalTex = finalTex + noise.r * (0.1 * sin(time));
    
    vec4 finalCol = texture(texture_data, finalTex);

    FragColor = finalCol;
}