#version 330 core
layout(location = 0) in vec2 aPos;
uniform mat3 model;
uniform mat3 projection;

void main() {
    vec3 pos = projection * model * vec3(aPos, 1.0);
    gl_Position = vec4(pos.xy, 0.0, 1.0);
}
