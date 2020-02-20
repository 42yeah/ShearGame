#version 330 core

in vec4 mvpPos;
in vec3 pos;
in vec3 normal;
in vec2 texCoord;

uniform sampler2D tex;

out vec4 color;


vec2 rand2d(vec2 uv) {
    return -1.0 + 2.0 * fract(sin(vec2(dot(uv, vec2(12.9898, 78.233)),
                                       dot(uv, vec2(31.516, 124.543)))) * 1000000.0);
}

float perlin(vec2 uv) {
    vec2 u = floor(uv);
    vec2 f = fract(uv);
    vec2 s = smoothstep(0.0, 1.0, f);
    
    vec2 a = rand2d(u);
    vec2 b = rand2d(u + vec2(1.0, 0.0));
    vec2 c = rand2d(u + vec2(0.0, 1.0));
    vec2 d = rand2d(u + vec2(1.0, 1.0));
    
    return mix(mix(dot(a, f), dot(b, f - vec2(1.0, 0.0)), s.x),
               mix(dot(c, f - vec2(0.0, 1.0)), dot(d, f - vec2(1.0, 1.0)), s.x),
               s.y) * 0.5 + 0.5;
}

vec3 mapGreeness(float p) {
    return mix(vec3(0.02, 0.1, 0.02), vec3(0.2, 0.3, 0.2), p);
}

vec3 fog(vec3 pos, vec3 col) {
    float far = 50.0;
    float fogness = pow(min(pos.z, far) / far, 0.5);
    return mix(col, vec3(0.0, 0.0, 0.0), fogness);
}

void main() {
    vec2 uv = pos.xz;
    uv *= 20.0;
    vec3 outputColor = mapGreeness(perlin(uv));
    color = vec4(fog(mvpPos.xyz, normal), 1.0);
}
