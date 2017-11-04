#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 MVP;

out vec2 fs_uv;
out float fs_t;

void main() {
	gl_Position = MVP * (gl_in[0].gl_Position + vec4(-50.0, -50.0, 0.0, 0.0));
	fs_uv = vec2(0.0, 1.0);
	fs_t = gl_in[0].gl_Position.z;
	EmitVertex();

	gl_Position = MVP * (gl_in[0].gl_Position + vec4(-50.0, 50.0, 0.0, 0.0));
	fs_uv = vec2(0.0, 0.0);
	fs_t = gl_in[0].gl_Position.z;
	EmitVertex();

	gl_Position = MVP * (gl_in[0].gl_Position + vec4(50.0, -50.0, 0.0, 0.0));
	fs_uv = vec2(1.0, 1.0);
	fs_t = gl_in[0].gl_Position.z;
	EmitVertex();

	gl_Position = MVP * (gl_in[0].gl_Position + vec4(50.0, 50.0, 0.0, 0.0));
	fs_uv = vec2(1.0, 0.0);
	fs_t = gl_in[0].gl_Position.z;
	EmitVertex();

	EndPrimitive();
}