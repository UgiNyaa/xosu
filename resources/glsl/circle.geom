#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

layout (std140) uniform shader_data {
	mat4 mvp;
	float time;
	float cs;
	float ar;
	float hw;
};

out vec2 fs_uv;
out float fs_t;

void main() {
	fs_t = gl_in[0].gl_Position.z;

	gl_Position = mvp * (gl_in[0].gl_Position + vec4(-cs, -cs, 0.0, 0.0));
	fs_uv = vec2(0.0, 1.0);
	EmitVertex();

	gl_Position = mvp * (gl_in[0].gl_Position + vec4(-cs, cs, 0.0, 0.0));
	fs_uv = vec2(0.0, 0.0);
	EmitVertex();

	gl_Position = mvp * (gl_in[0].gl_Position + vec4(cs, -cs, 0.0, 0.0));
	fs_uv = vec2(1.0, 1.0);
	EmitVertex();

	gl_Position = mvp * (gl_in[0].gl_Position + vec4(cs, cs, 0.0, 0.0));
	fs_uv = vec2(1.0, 0.0);
	EmitVertex();

	EndPrimitive();
}