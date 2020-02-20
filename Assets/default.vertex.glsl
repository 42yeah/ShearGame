#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model, view, perspective;

out vec3 pos;
out vec4 mvpPos;
out vec3 normal;
out vec2 texCoord;


void main() {
    mvpPos = perspective * view * model * vec4(aPos, 1.0);
    gl_Position = mvpPos;
    pos = aPos;
    normal = aNormal;
    texCoord = aTexCoord;
}
