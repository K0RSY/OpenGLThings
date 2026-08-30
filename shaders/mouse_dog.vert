#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNor;
layout (location = 2) in vec2 aTex;

out vec3 normal;
out float Zbf;
out float vNor;
out vec3 wPos;
out vec2 Tex;

#define light_count 2

uniform mat4 normal_model;
uniform vec4 global_light;
uniform vec3 camera_pos;
uniform vec4 lights[light_count];
uniform float ambient_light;

uniform float time;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
   vec4 Pos = vec4(aPos, 1.f);

   Pos = model * Pos;
   wPos = Pos.xyz;
   
   normal = normalize(mat3(normal_model) * aNor);
   vNor = ambient_light + max(dot(normal, global_light.xyz), 0) * global_light.w;
   for (int i = 0; i < light_count; i++) {
      vNor += max(dot(normal, normalize(lights[i].xyz - wPos)), 0) * lights[i].w;
   }
   
   // float coeff = sin(time * 5 + Pos.x / 2 + Pos.y / 2) / 5;
   // Pos = vec4(Pos.x + coeff, Pos.y + coeff, Pos.z, 1.f);
   Pos = projection * view * Pos;
   
   gl_Position = Pos;

   Zbf = Pos.z;
   Tex = vec2(aTex.x, 1 - aTex.y);
}