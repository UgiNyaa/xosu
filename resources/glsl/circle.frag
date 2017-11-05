#version 330 core

in vec2 fs_uv;
in float fs_t;

layout (std140) uniform shader_data {
	mat4 mvp;
	float time;
	float cs;
	float ar;
	float hw;
};

uniform sampler2D tex;

out vec4 color;

void main() {
	color = texture(tex, fs_uv);
	color.a *= smoothstep(fs_t - ar, fs_t - ar + 0.1, time) - smoothstep(fs_t + hw, fs_t + hw, time);

	if (color.a < 0.1)
		discard;
}
