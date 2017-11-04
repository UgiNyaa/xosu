#version 330 core

in vec2 fs_uv;
in float fs_t;

uniform sampler2D tex;
uniform float time;

out vec4 color;

void main() {
	color = texture(tex, fs_uv);
	color.a *= smoothstep(fs_t - 1.0, fs_t - 0.9, time) - smoothstep(fs_t, fs_t + 0.3, time);

	if (color.a < 0.1)
		discard;
}
