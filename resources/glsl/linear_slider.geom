#version 330 core

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

layout (std140) uniform shader_data {
	mat4 mvp;
	float time;
	float cs;
	float ar;
	float hw;
};

void main() {
	//vec4 pos = gl_in[1].gl_Position;
	//vec4 min_min = pos + vec4(-cs, -cs, 0.0, 0.0);
	//vec4 min_max = pos + vec4(-cs, +cs, 0.0, 0.0);
	//vec4 max_min = pos + vec4(+cs, -cs, 0.0, 0.0);
	//vec4 max_max = pos + vec4(+cs, +cs, 0.0, 0.0);

	vec4 bgn = gl_in[0].gl_Position;
	vec4 end = gl_in[1].gl_Position;
	vec4 min_min = vec4(
		min(bgn.x, end.x) - cs,
		min(bgn.y, end.y) - cs,
		bgn.z, bgn.w
	);
	vec4 min_max = vec4(
		min(bgn.x, end.x) - cs,
		max(bgn.y, end.y) + cs,
		bgn.z, bgn.w
	);
	vec4 max_min = vec4(
		max(bgn.x, end.x) + cs,
		min(bgn.y, end.y) - cs,
		bgn.z, bgn.w
	);
	vec4 max_max = vec4(
		max(bgn.x, end.x) + cs,
		max(bgn.y, end.y) + cs,
		bgn.z, bgn.w
	);

	gl_Position = mvp * min_min;
	EmitVertex();

	gl_Position = mvp * min_max;
	EmitVertex();

	gl_Position = mvp * max_min;
	EmitVertex();

	gl_Position = mvp * max_max;
	EmitVertex();

	EndPrimitive();
}