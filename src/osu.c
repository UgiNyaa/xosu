#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "gl-matrix.h"
#include "incbin.h"

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

INCBIN(VertexShader, "glsl/vs.glsl");
INCBIN(GeometryShader, "glsl/gs.glsl");
INCBIN(FragmentShader, "glsl/fs.glsl");
GLuint init_shader_program() {
	GLint success = 0, logSize = 0;
	GLuint vsID = glCreateShader(GL_VERTEX_SHADER);
	GLuint gsID = glCreateShader(GL_GEOMETRY_SHADER);
	GLuint fsID = glCreateShader(GL_FRAGMENT_SHADER);

	const char* vs_code[1] = {gVertexShaderData};
	glShaderSource(vsID, 1, vs_code, &gVertexShaderSize);
	glCompileShader(vsID);

	glGetShaderiv(vsID, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		glGetShaderiv(vsID, GL_INFO_LOG_LENGTH, &logSize);

		char* log = malloc(logSize);
		glGetShaderInfoLog(vsID, logSize, &logSize, log);

		printf("vs");
		printf(log);
		free(log);

		exit(-1);
	}

	const char* gs_code[1] = {gGeometryShaderData};
	glShaderSource(gsID, 1, gs_code, &gGeometryShaderSize);
	glCompileShader(gsID);

	glGetShaderiv(gsID, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		glGetShaderiv(gsID, GL_INFO_LOG_LENGTH, &logSize);

		char* log = malloc(logSize);
		glGetShaderInfoLog(gsID, logSize, &logSize, log);

		printf("gs");
		printf(log);
		free(log);

		exit(-1);
	}

	const char* fs_code[1] = {gFragmentShaderData};
	glShaderSource(fsID, 1, fs_code, &gFragmentShaderSize);
	glCompileShader(fsID);

	glGetShaderiv(fsID, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		glGetShaderiv(fsID, GL_INFO_LOG_LENGTH, &logSize);

		char* log = malloc(logSize);
		glGetShaderInfoLog(fsID, logSize, &logSize, log);

		printf("fs");
		printf(log);
		free(log);

		exit(-1);
	}


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