#version 330 core

layout(location = 0) in vec2 vs_circles;
layout(location = 1) in float vs_hittime;

uniform mat4 MVP;

out float gs_hittime;

void main() {
	gl_Position = MVP * vec4(vs_circles, 0.0, 1.0);

	gs_hittime = vs_hittime;
}

