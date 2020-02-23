#version 330 core

in vec2 uv;

uniform sampler2D tex;
uniform float aspect;

out vec4 color;


vec3 gamma(vec3 i) {
    return pow(i, vec3(1.0 / 2.2));
}

vec3 dottify(vec3 i) {
    vec2 u = -1.0 + 2.0 * uv;
    u.x *= aspect;
    float l = min(pow(length(u) + 0.99, 100.0), 1.0);
    
    return mix(1.0 - i, i, l);
}

void main() {
    vec2 u = uv;
    
    vec3 texColor = texture(tex, u).rgb;
    
    color = vec4(dottify(gamma(texColor)), 1.0);
//    color = texture(shadow, u);
}
