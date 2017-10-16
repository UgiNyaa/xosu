#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in float gs_hittime[];

uniform mat4 MVP;

flat out float fs_hittime;

void main() {
	fs_hittime = gs_hittime[0];

	gl_Position = gl_in[0].gl_Position + MVP * vec4(-2.5, -2.5, 0.0, 0.0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + MVP * vec4(-2.5, 2.5, 0.0, 0.0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + MVP * vec4(2.5, -2.5, 0.0, 0.0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + MVP * vec4(2.5, 2.5, 0.0, 0.0);
	EmitVertex();

	EndPrimitive();
}

