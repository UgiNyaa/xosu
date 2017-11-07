#version 330 core

//in vec2 fs_uv;
//in vec2 fs_uv_begin;
//in vec2 fs_uv_end;

//in float fs_t_begin;
//in float fs_t_end;

layout(std140) uniform shader_data {
	mat4 mvp;
	float time;
	float cs;
	float ar;
	float hw;
};

out vec4 color;

void main() {
	color = vec4(1.0);
}
