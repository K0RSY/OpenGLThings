#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec2 Tex;

uniform mat4 billboard;
uniform mat4 view;
uniform mat4 projection;

void main() {
   vec4 Pos = vec4(aPos, 1.f);

   Pos = projection * view * Pos;
   
   gl_Position = Pos;

   Tex = aTex;
}