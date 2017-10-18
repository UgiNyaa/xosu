#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 MVP;

out vec2 fs_uv;
out float fs_z;

void main() {
	fs_z = gl_in[0].gl_Position.z;

	gl_Position = gl_in[0].gl_Position + MVP * vec4(-5.0, -5.0, 0.0, 0.0);
	fs_uv = vec2(0.0, 1.0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + MVP * vec4(-5.0, 5.0, 0.0, 0.0);
	fs_uv = vec2(0.0, 0.0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + MVP * vec4(5.0, -5.0, 0.0, 0.0);
	fs_uv = vec2(1.0, 1.0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + MVP * vec4(5.0, 5.0, 0.0, 0.0);
	fs_uv = vec2(1.0, 0.0);
	EmitVertex();

	EndPrimitive();
}

