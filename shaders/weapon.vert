#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec2 Tex;

uniform float sin_time_x;
uniform float sin_time_y;

void main() {
   vec3 Pos = aPos;

   Pos.y -= sin_time_y;
   Pos.x -= sin_time_x;

   gl_Position = vec4(Pos, 1.f);

   Tex = aTex;
}