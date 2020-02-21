#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model, view, perspective;

uniform vec3 offset;

out vec2 uv;


void main() {
    mat4 mv = view * model;
    mat4 v = mat4(vec4(1.0, mv[1][0], 0.0, mv[0][3]),
                  vec4(0.0, mv[1][1], 0.0, mv[1][3]),
                  vec4(0.0, mv[1][2], 1.0, mv[2][3]),
                  mv[3]);
    gl_Position = perspective * v * vec4(aPos, 1.0);
    uv = vec2(aPos.x + 0.5, aPos.y / 2.0);
}
