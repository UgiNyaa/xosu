#version 330 core

in vec2 fs_uv;
in float fs_z;

uniform sampler2D circleTexture;

out vec4 color;

float easeInCubic() {
	return max(0, min(1, (fs_z * 10) * (fs_z * 10) * (fs_z * 10)));
}

void main() {
	color = texture(circleTexture, fs_uv);
	color.a = color.a * fs_z;
}

