#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
layout (location = 2) in vec2 aTex;

out vec3 Col;
out float Zbf;
out vec2 Tex;

uniform float time;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
   vec4 Pos = vec4(aPos, 1.f);

   // float coeff = (abs(sin(time * 2 + Pos.x / 3 + Pos.y / 3)) - .5) / 5;
   float coeff = sin(time * 5 + Pos.x / 2 + Pos.y / 2) / 5;

   Pos = model * Pos;
   // Pos = billboard * Pos;
   // float coeff = sin(Pos.x / 2 + Pos.y / 2) / 5;
   // Pos = vec4(Pos.x + coeff, Pos.y + coeff, Pos.z, 1.f);
   Pos = projection * view * Pos;
   
   gl_Position = Pos;

   Col = aCol;
   Zbf = Pos.z;
   Tex = aTex;
}