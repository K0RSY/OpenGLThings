#version 330 core

in vec3 normal;
in float vNor;
in float Zbf;
in vec2 Tex;
in vec3 wPos;

out vec4 FragColor;

#define light_count 2

uniform vec4 global_light;
uniform vec3 camera_pos;
uniform vec4 lights[light_count];
uniform float ambient_light;

uniform float time;

uniform sampler2D texture_data;
uniform sampler2D texture_datb;

void main() {
    vec2 finalTex = Tex;

    vec4 noise = texture(texture_datb, Tex);

    // finalTex = finalTex + sin(finalTex * 8) / (10 + ((sin(time / 100) + 1) / 2) * 50);
    // finalTex = finalTex + noise.r * (0.1 * sin(time));
    
    vec4 finalCol = texture(texture_data, finalTex);

    finalCol.a *= min(((Zbf) * 3), 1);

    float Nor = vNor;
    Nor += pow(max(dot(reflect(normalize(-global_light.xyz), normal), normalize(camera_pos - wPos)), 0), 16) * global_light.w;
    for (int i = 0; i < light_count; i++) {
        Nor += pow(max(dot(reflect(normalize(wPos - lights[i].xyz), normal), normalize(camera_pos - wPos)), 0), 16) * lights[i].w;
    }

    finalCol.rgb *= Nor;

    if (finalCol.a < 0.1) {
        discard;
    }

    FragColor = finalCol;
}