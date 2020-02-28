#version 330 core

in vec2 uv;

uniform sampler2D tex;
uniform float aspect;
uniform float bedCounter;
uniform float hallucinating;

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

void main() {
    vec2 u = uv;
    
    vec3 texColor = texture(tex, u).rgb;
    
    color = vec4(dottify(gamma(texColor)), 1.0);
    
    u = -1.0 + 2.0 * u;
    float bc = (1.0 - max(0.0, min(1.0, bedCounter))) * 10.0;
    float dCenter = 1.0 - abs(u.y) * bc;
    color.rgb *= dCenter * max(0.0, min(1.0, bedCounter));

    u.x += sin(hallucinating * 2.0);
    u.y += cos(hallucinating) * 0.5;
    vec3 a = vec3(uv.x, uv.y, uv.x / 2.0);
    vec3 b = vec3(uv.y, uv.x, uv.y / 2.0);
    vec3 hallu = mix(a, b, perlin(u * 10.0));
    color.rgb = mix(mix(color.rgb, 1.0 - color.rgb, hallucinating / 100.0), hallu, sin(hallucinating));
//    color = texture(shadow, u);
}
