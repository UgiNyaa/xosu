#version 330 core

layout(location = 0) in vec3 vs_pos;

layout(std140) uniform shader_data {
	mat4 mvp;
	float time;
	float cs;
	float ar;
	float hw;
};

void main() {
	gl_Position = vec4(vs_pos, 1.0);
}
