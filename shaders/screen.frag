#version 330 core

in vec2 Tex;

out vec4 FragColor;

#define OUTLINE_THRESHOLD 0.002f
#define BIG 100000

uniform sampler2D texture_data;
uniform sampler2D texture_datb;

uniform float crt;
uniform float outline_width;
uniform float outline_height;

void main() {
    vec2 finalTex = Tex;
    
    float pi = 3.1415926;

    finalTex.y -= (Tex.y * 2 - 1) * (sin(Tex.x * pi) - 1.2) * crt;
    finalTex.x -= (Tex.x * 2 - 1) * (sin(Tex.y * pi) - 1.2) * crt;

    float lineTint = floor(mod((finalTex.y * 200), 2)) * .1 + .9;

    vec4 finalCol = texture(texture_data, finalTex);
    
    float base_brightness = texture(texture_datb, finalTex).r;
    float outline = 0.f;
    outline += max(-(base_brightness + OUTLINE_THRESHOLD) + texture(texture_datb, finalTex + vec2( outline_width,  0)).r, 0.f);
    outline += max(-(base_brightness + OUTLINE_THRESHOLD) + texture(texture_datb, finalTex + vec2(-outline_width,  0)).r, 0.f);
    outline += max(-(base_brightness + OUTLINE_THRESHOLD) + texture(texture_datb, finalTex + vec2(0,               outline_height)).r, 0.f);
    outline += max(-(base_brightness + OUTLINE_THRESHOLD) + texture(texture_datb, finalTex + vec2(0,              -outline_height)).r, 0.f);

    outline += max(-(base_brightness + OUTLINE_THRESHOLD) + texture(texture_datb, finalTex + vec2( outline_width,  outline_height)).r, 0.f);
    outline += max(-(base_brightness + OUTLINE_THRESHOLD) + texture(texture_datb, finalTex + vec2(-outline_width, -outline_height)).r, 0.f);
    outline += max(-(base_brightness + OUTLINE_THRESHOLD) + texture(texture_datb, finalTex + vec2(-outline_width,  outline_height)).r, 0.f);
    outline += max(-(base_brightness + OUTLINE_THRESHOLD) + texture(texture_datb, finalTex + vec2( outline_width, -outline_height)).r, 0.f);

    finalCol.xyz *= vec3(1.f, 1.f, 1.f) + min(outline * BIG, 1.f) * vec3(1.f, 1.f, 1.f);

    finalCol.xyz *= lineTint;

    FragColor = finalCol;
}