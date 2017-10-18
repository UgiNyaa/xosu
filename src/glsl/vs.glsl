#version 330 core

layout(location = 0) in vec3 vs_circles;

uniform mat4 MVP;

void main() {
	gl_Position = MVP * vec4(vs_circles, 1.0);
}

