#version 330 core

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

layout(std140) uniform shader_data {
	mat4 mvp;
	float time;
	float cs;
	float ar;
	float hw;
};

//out vec2 fs_uv;
//out vec2 fs_uv_begin;
//out vec2 fs_uv_end;

//out float fs_t_begin;
//out float fs_t_end;

void main() {
	float maxx = max(gl_in[0].gl_Position.x+cs, gl_in[1].gl_Position.x+cs);
	float minx = min(gl_in[0].gl_Position.x-cs, gl_in[1].gl_Position.x-cs);
	float maxy = max(gl_in[0].gl_Position.y+cs, gl_in[1].gl_Position.y+cs);
	float miny = min(gl_in[0].gl_Position.y-cs, gl_in[1].gl_Position.y-cs);
	float minz = gl_in[0].gl_Position.z;
	float maxz = gl_in[1].gl_Position.z;

	//fs_uv_begin = vec2(
	//	(gl_in[0].gl_Position.x - minx) / (maxx - minx),
	//	(gl_in[0].gl_Position.y - miny) / (maxy - miny)
	//);
	//fs_uv_end = vec2(
	//	(gl_in[1].gl_Position.x - minx) / (maxx - minx),
	//	(gl_in[1].gl_Position.y - miny) / (maxy - miny)
	//);
	//fs_t_begin = gl_in[0].gl_Position.z;
	//fs_t_end = gl_in[1].gl_Position.z;
	
	gl_Position = mvp * vec4(minx, miny, minz, 0.0);
	//fs_uv = vec2(0.0, 0.0);
	EmitVertex();

	gl_Position = mvp * vec4(minx, maxy, minz, 0.0);
	//fs_uv = vec2(0.0, 1.0);
	EmitVertex();

	gl_Position = mvp * vec4(maxx, miny, minz, 0.0);
	//fs_uv = vec2(1.0, 0.0);
	EmitVertex();

	gl_Position = mvp * vec4(maxx, maxy, minz, 0.0);
	//fs_uv = vec2(1.0, 1.0);
	EmitVertex();

	EndPrimitive();
}
