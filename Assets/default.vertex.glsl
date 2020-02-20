#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model, view, perspective;

out vec3 pos;
out vec4 mvpPos;


void main() {
    mvpPos = perspective * view * model * vec4(aPos, 1.0);
    gl_Position = mvpPos;
    pos = aPos;
}
