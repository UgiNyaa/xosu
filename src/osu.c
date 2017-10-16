#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "gl-matrix.h"

#define WIDTH 1280
#define HEIGHT 720

GLuint init_shader_program();
GLuint init_vao(GLuint* vertexbuffer, GLuint* elementbuffer);

int main(int argc, char* argv[]) {
	if (!glfwInit()) {
		printf("Failed to initialize GLFW\n");
		return 1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window;

	window = glfwCreateWindow(WIDTH, HEIGHT, "Tutorial 01", NULL, NULL);

	if (window == NULL) {
		printf("Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials\n");
		return 1;
	}

	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		printf("Failed to initialize GLEW\n");
		return 1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLuint pID = init_shader_program();

	GLuint circlebuffer, timebuffer;
	GLuint vao = init_vao(&circlebuffer, &timebuffer);

	GLuint mvpID = glGetUniformLocation(pID, "MVP");
	GLuint tID = glGetUniformLocation(pID, "time");

	glfwSetTime(0.0);
	for (;;) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float projection[16];
		mat4_ortho(-WIDTH/2.0f/10, WIDTH/2.0f/10, -HEIGHT/2.0f/10, HEIGHT/2.0f/10, 0.1, 10, projection);
		//mat4_perspective(1.5f, 16.0f / 9.0f, 1.0f, 1000.0f, projection);

		float eye[3] = { 0.0f, 0.0f, 5.0f };
		float center[3] = { 0.0f, 0.0f, 0.0f };
		float up[3] = { 0.0f, 1.0f, 0.0f };
		float view[16];
		mat4_lookAt(eye, center, up, view);

		float mvp[16];
		mat4_multiply(projection, view, mvp);

		glUseProgram(pID);

		glUniformMatrix4fv(mvpID, 1, GL_FALSE, mvp);
		glUniform1f(tID, (float)glfwGetTime());

		glBindVertexArray(vao);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glDrawArrays(GL_POINTS, 0, 3);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

GLuint init_shader_program() {
	GLint success = 0, logSize = 0;
	GLuint vsID = glCreateShader(GL_VERTEX_SHADER);
	GLuint gsID = glCreateShader(GL_GEOMETRY_SHADER);
	GLuint fsID = glCreateShader(GL_FRAGMENT_SHADER);

	const char* vertex_shader_code =
		"#version 330 core\n"
		"\n"
		"layout(location = 0) in vec2 vs_circles;\n"
		"layout(location = 1) in float vs_hittime;\n"
		"\n"
		"uniform mat4 MVP;\n"
		"\n"
		"out float gs_hittime;\n"
		"\n"
		"void main() {\n"
		"	gl_Position = MVP * vec4(vs_circles, 0.0, 1.0);\n"
		"	\n"
		"	gs_hittime = vs_hittime;\n"
		"}\n\n";
	glShaderSource(vsID, 1, &vertex_shader_code, NULL);
	glCompileShader(vsID);

	glGetShaderiv(vsID, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		glGetShaderiv(vsID, GL_INFO_LOG_LENGTH, &logSize);

		char* log = malloc(logSize);
		glGetShaderInfoLog(vsID, logSize, &logSize, log);

		printf(log);
		free(log);

		exit(-1);
	}

	printf(vertex_shader_code);

	const char* geometry_shader_code =
		"#version 330 core\n"
		"\n"
		"layout(points) in;\n"
		"layout(triangle_strip, max_vertices = 4) out;\n"
		"\n"
		"in float gs_hittime[];\n"
		"\n"
		"uniform mat4 MVP;\n"
		"\n"
		"flat out float fs_hittime;\n"
		"\n"
		"void main() {\n"
		"	fs_hittime = gs_hittime[0];\n"
		"	\n"
		"	gl_Position = gl_in[0].gl_Position + MVP * vec4(-2.5, -2.5, 0.0, 0.0);\n"
		"	EmitVertex();\n"
		"	\n"
		"	gl_Position = gl_in[0].gl_Position + MVP * vec4(-2.5, 2.5, 0.0, 0.0);\n"
		"	EmitVertex();\n"
		"	\n"
		"	gl_Position = gl_in[0].gl_Position + MVP * vec4(2.5, -2.5, 0.0, 0.0);\n"
		"	EmitVertex();\n"
		"	\n"
		"	gl_Position = gl_in[0].gl_Position + MVP * vec4(2.5, 2.5, 0.0, 0.0);\n"
		"	EmitVertex();\n"
		"	\n"
		"	EndPrimitive();\n"
		"}\n\n";
	glShaderSource(gsID, 1, &geometry_shader_code, NULL);
	glCompileShader(gsID);

	glGetShaderiv(gsID, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		glGetShaderiv(gsID, GL_INFO_LOG_LENGTH, &logSize);

		char* log = malloc(logSize);
		glGetShaderInfoLog(gsID, logSize, &logSize, log);

		printf(log);
		free(log);

		exit(-1);
	}

	printf(geometry_shader_code);

	const char* fragment_shader_code =
		"#version 330 core\n"
		"\n"
		"flat in float fs_hittime;\n"
		"\n"
		"uniform float time;\n"
		"\n"
		"out vec4 color;\n"
		"\n"
		"void main() {\n"
		"	float sigma = 0.2;\n"
		"	float left = (1.0/(2.5*sigma));\n"
		"	float right = exp( -(1.0/2.0)*((time-fs_hittime)/sigma)*((time-fs_hittime)/sigma) );\n"
		"	color = vec4(1.0, 1.0, 1.0, 1.0);\n"
		"	color.a = left * exp( -(1.0/2.0) * ((fs_hittime-time)/sigma)*((fs_hittime-time)/sigma) );\n"
		//"	color.a = 0.5;\n"
		"}\n\n";
	glShaderSource(fsID, 1, &fragment_shader_code, NULL);
	glCompileShader(fsID);

	glGetShaderiv(fsID, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		glGetShaderiv(fsID, GL_INFO_LOG_LENGTH, &logSize);

		char* log = malloc(logSize);
		glGetShaderInfoLog(fsID, logSize, &logSize, log);

		printf(log);
		free(log);

		exit(-1);
	}

	printf(fragment_shader_code);

	GLuint pID = glCreateProgram();
	glAttachShader(pID, vsID);
	glAttachShader(pID, gsID);
	glAttachShader(pID, fsID);
	glLinkProgram(pID);

	return pID;
}

GLuint init_vao(GLuint* circlebuffer, GLuint* timebuffer) {
	GLuint vao;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLfloat circle_data[6] = {
		0.0f, 0.0f,
		5.0f, 5.0f,
		-5.0f, -5.0f
	};
	glGenBuffers(1, circlebuffer);
	glBindBuffer(GL_ARRAY_BUFFER, *circlebuffer);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(circle_data),
		&circle_data[0],
		GL_STATIC_DRAW
	);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// corrupted data!
	GLfloat time_data[3] = { 3.0f, 5.0f, 8.0f };
	glGenBuffers(1, timebuffer);
	glBindBuffer(GL_ARRAY_BUFFER, *timebuffer);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(time_data),
		&time_data[0],
		GL_STATIC_DRAW
	);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	
	return vao;
}