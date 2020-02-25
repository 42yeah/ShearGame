#version 330 core

in vec2 uv;

uniform sampler2D tex;
uniform float aspect;
uniform float bedCounter;

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
    
    u = -1.0 + 2.0 * u;
    float bc = (1.0 - max(0.0, min(1.0, bedCounter))) * 10.0;
    float dCenter = 1.0 - abs(u.y) * bc;
    color.rgb *= dCenter * max(0.0, min(1.0, bedCounter));
//    color = texture(shadow, u);
}
