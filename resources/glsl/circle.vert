#version 330 core

layout(location = 0) in vec2 vs_pos;
layout(location = 1) in vec2 vs_uv;

uniform mat4 MVP;

out vec2 fs_uv;

void main() {
	gl_Position = MVP * vec4(vs_pos, 0.0, 1.0);

    fs_uv = vs_uv;
}
