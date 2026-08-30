#version 330 core

in vec3 normal;
in float Zbf;
in vec2 Tex;
in vec3 wPos;

out vec4 FragColor;

#define light_count 2

uniform mat4 normal_model;
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

    finalCol.a *= ((Zbf) * 3);

    float Nor = 0;
    Nor = ambient_light + max(dot(normal, global_light.xyz), 0) * global_light.w;

    for (int i = 0; i < light_count; i++) {
        Nor += max(dot(normal, normalize(lights[i].xyz - wPos)), 0) * lights[i].w;
        Nor += pow(max(dot(reflect(normalize(wPos - lights[i].xyz), normal), normalize(camera_pos - wPos)), 0), 16) * lights[i].w * 2;
    }

    finalCol.rgb *= Nor;

    FragColor = finalCol;
}