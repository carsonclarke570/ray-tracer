#version 430

uniform sampler2D render_tex;

in vec2 texcoord;

out vec4 color;

void main() {
    color = vec4(texture(render_tex, texcoord).xyz, 1.0f);
}