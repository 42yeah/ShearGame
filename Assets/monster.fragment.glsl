#version 330 core

in vec2 uv;

uniform int id;
uniform sampler2D tex;

struct Sun {
    vec3 dir;
    vec3 color;
};
uniform Sun sun;

out vec4 color;


void main() {
    vec2 u = uv * 1.0;
    int count = 4;
    int idd = id;
    float w = 1.0 / float(count);
    int row = (count - 1) - idd / count;
    int col = idd % count;
    vec2 offset = vec2(col * w, row * w);
    color = texture(tex, offset + u * vec2(w, w));
//    color = texture(tex, uv);
    if (color.a == 0.0) {
        discard;
    }
    color *= vec4(sun.color, 1.0);
}
