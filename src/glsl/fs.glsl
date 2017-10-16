#version 330 core

flat in float fs_hittime;

uniform float time;

out vec4 color;

void main() {
	float sigma = 0.2;
	color = vec4(1.0, 1.0, 1.0f, 1.0f);
	color.a = (1.0/(2.5*sigma)) * exp( -0.5 * ((fs_hittime-time)/sigma)*((fs_hittime-time)/sigma) );
}

