#version 330 core

layout(location = 0) in vec3 vs_pos;

uniform mat4 MVP;

void main() {
	gl_Position = vec4(vs_pos, 1.0);
}
