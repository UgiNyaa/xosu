#version 330 core

in vec2 fs_uv;

uniform sampler2D circleTexture;

out vec4 color;

void main() {
	color = texture(circleTexture, fs_uv);
}

