#version 330 core

layout (std140) uniform shader_data {
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
