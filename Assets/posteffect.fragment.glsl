#version 330 core

in vec2 uv;

uniform sampler2D tex;

out vec4 color;


vec3 gamma(vec3 i) {
    return pow(i, vec3(1.0 / 2.2));
}

void main() {
    vec2 u = uv;
    u = floor(u * 100.0);
    vec3 texColor = texture(tex, u / 100.0).rgb;
    
    color = vec4(gamma(texColor), 1.0);
}