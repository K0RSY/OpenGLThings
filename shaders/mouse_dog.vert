#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNor;
layout (location = 2) in vec2 aTex;

out float Nor;
out float Zbf;
out vec2 Tex;

uniform float time;
uniform float ambient_light;

uniform vec4 global_light;
uniform vec4 light;

uniform mat4 model;
uniform mat4 normal_model;
uniform mat4 view;
uniform mat4 projection;

void main() {
   vec4 Pos = vec4(aPos, 1.f);

   // float coeff = (abs(sin(time * 2 + Pos.x / 3 + Pos.y / 3)) - .5) / 5;
   float coeff = sin(time * 5 + Pos.x / 2 + Pos.y / 2) / 5;

   Pos = model * Pos;

   vec3 normal = mat3(normal_model) * aNor;
   Nor =
      ambient_light
      + max(dot(normal, global_light.xyz), 0) * global_light.w
      + max(dot(normal, light.xyz - Pos.xyz), 0) * light.w;

   // Pos = billboard * Pos;
   // float coeff = sin(Pos.x / 2 + Pos.y / 2) / 5;
   // Pos = vec4(Pos.x + coeff, Pos.y + coeff, Pos.z, 1.f);
   Pos = projection * view * Pos;
   
   gl_Position = Pos;

   Zbf = Pos.z;
   Tex = aTex;
}