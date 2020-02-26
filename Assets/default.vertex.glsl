#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform float destroyness;
uniform mat4 model, view, perspective;

out vec3 pos;
out vec4 mvpPos;
out vec3 normal;
out vec2 texCoord;


void main() {
    pos = aPos;
    normal = aNormal;
    texCoord = aTexCoord;
    pos.y -= destroyness * 2.0f;
    mvpPos = perspective * view * model * vec4(pos, 1.0);
    gl_Position = mvpPos;
}
