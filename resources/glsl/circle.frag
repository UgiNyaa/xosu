#version 330 core

in vec2 fs_uv;

uniform sampler2D tex;

out vec4 color;

void main() {
	color = texture(tex, fs_uv);
}
