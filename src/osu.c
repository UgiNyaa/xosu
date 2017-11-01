#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "gl-matrix.h"
#include "lodepng.h"
#include "circle_buffer.h"

#define WIDTH 1280
#define HEIGHT 720

GLuint init_program(const char* vs_path, const char* fs_path);
GLuint init_circle_program(GLuint* vao, GLuint* vbo_pos, GLuint* vbo_uv);

void init_textures(GLuint* tex, GLenum texnum, const char* path);

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

	window = glfwCreateWindow(WIDTH, HEIGHT, "xosu", NULL, NULL);

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

	GLuint circle_vao, circle_pos, circle_uv;
	GLuint circle_program = init_circle_program(&circle_vao, &circle_pos, &circle_uv);

	GLuint hitc_tex, appr_tex;
	init_textures(&hitc_tex, GL_TEXTURE0, "resources/textures/hitcircleoverlay@2x.png");
	init_textures(&hitc_tex, GL_TEXTURE1, "resources/textures/approachcircle@2x.png");

	GLuint mvpID = glGetUniformLocation(circle_program, "MVP");
	GLuint texID = glGetUniformLocation(circle_program, "tex");

	int circle_off = 0;
	int circle_span = 0;
	int ar = 5000;

	glfwSetTime(0.0);
	for (;;) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float projection[16];
		mat4_ortho(
			-WIDTH/20.0f, 
			WIDTH/20.0f, 
			-HEIGHT/20.0f, 
			HEIGHT/20.0f, 
			0.0f, 
			100000.0f, 
			projection
		);

		float eye[3] = { 0.0f, 0.0f, 0.0f };
		float center[3] = { 0.0f, 0.0f, 1.0f };
		float up[3] = { 0.0f, 1.0f, 0.0f };
		float view[16];
		mat4_lookAt(eye, center, up, view);

		glUseProgram(circle_program);
		glBindVertexArray(circle_vao);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glUniform1i(texID, 0);

		int ms = (int)(glfwGetTime() * 1000);
		if (circle_buffer[circle_off + circle_span].t - ar < ms)
			circle_span++;
		else if (circle_buffer[circle_off].t + 250 < ms) {
			circle_span--;
			circle_off++;
		}

		// circle drawing
		for (int i = circle_off + circle_span - 1; i >= circle_off; i--) {
			float circle_model[16];
			float mvp[16];

			mat4_identity(circle_model);
			mat4_identity(mvp);

			float translation[3] = { 
				circle_buffer[i].x,
				circle_buffer[i].y,
				circle_buffer[i].t / 1000.0f
			};
			mat4_translate(circle_model, translation, circle_model);

			mat4_identity(mvp);
			mat4_multiply(projection, view, mvp);
			mat4_multiply(mvp, circle_model, mvp);

			glUniform1i(texID, 0);
			glUniformMatrix4fv(mvpID, 1, GL_FALSE, mvp);

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			if (circle_buffer[i].t > ms)
			{
				float approach_model[16];
				mat4_identity(approach_model);

				translation[0] = 0.0f;
				translation[1] = 0.0f;
				translation[2] = .1f;
				mat4_translate(circle_model, translation, circle_model);

				float scale[3] = {
					1.0f + 2.0f * (float)(circle_buffer[i].t - ms) / ar,
					1.0f + 2.0f * (float)(circle_buffer[i].t - ms) / ar,
					1.0f + 2.0f * (float)(circle_buffer[i].t - ms) / ar
				};
				mat4_scale(circle_model, scale, approach_model);

				mat4_identity(mvp);
				mat4_multiply(projection, view, mvp);
				mat4_multiply(mvp, approach_model, mvp);

				glUniform1i(texID, 1);
				glUniformMatrix4fv(mvpID, 1, GL_FALSE, mvp);

				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			}
		}

		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

GLuint init_program(const char* vs_path, const char* fs_path) {
	GLint success = 0, logSize = 0;
	GLuint vsID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fsID = glCreateShader(GL_FRAGMENT_SHADER);

	FILE* fvs = fopen(vs_path, "rb");
	FILE* ffs = fopen(fs_path, "rb");

	fseek(fvs, 0L, SEEK_END);
	fseek(ffs, 0L, SEEK_END);

	int vs_size = ftell(fvs);
	int fs_size = ftell(ffs);

	fseek(fvs, 0L, SEEK_SET);
	fseek(ffs, 0L, SEEK_SET);

	char* vs_code = malloc(vs_size * sizeof(char) + 1);
	char* fs_code = malloc(fs_size * sizeof(char) + 1);

	fread(vs_code, sizeof(char), vs_size, fvs);
	fread(fs_code, sizeof(char), fs_size, ffs);
	vs_code[vs_size] = '\0';
	fs_code[fs_size] = '\0';

	fclose(fvs);
	fclose(ffs);

	glShaderSource(vsID, 1, &vs_code, &vs_size);
	glCompileShader(vsID);

	glGetShaderiv(vsID, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		glGetShaderiv(vsID, GL_INFO_LOG_LENGTH, &logSize);

		char* log = malloc(logSize);
		glGetShaderInfoLog(vsID, logSize, &logSize, log);

		printf("vs: \n");
		printf(log);
		free(log);

		exit(-1);
	}

	glShaderSource(fsID, 1, &fs_code, &fs_size);
	glCompileShader(fsID);

	glGetShaderiv(fsID, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		glGetShaderiv(fsID, GL_INFO_LOG_LENGTH, &logSize);

		char* log = malloc(logSize);
		glGetShaderInfoLog(fsID, logSize, &logSize, log);

		printf("fs: \n");
		printf(log);
		free(log);

		exit(-1);
	}


	GLuint pID = glCreateProgram();
	glAttachShader(pID, vsID);
	glAttachShader(pID, fsID);
	glLinkProgram(pID);

	free(vs_code);
	free(fs_code);

	return pID;
}

GLuint init_circle_program(GLuint* vao, GLuint* vbo_pos, GLuint* vbo_uv) {
	glGenVertexArrays(1, vao);
	glBindVertexArray(*vao);

	float vertex_data[8] = {
		// point down left
		-5.0f, -5.0f,
		// point up left
		-5.0f, +5.0f,
		// point down right
		+5.0f, -5.0f,
		// point up right
		+5.0f, +5.0f
	};

	glGenBuffers(1, vbo_pos);
	glBindBuffer(GL_ARRAY_BUFFER, *vbo_pos);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(vertex_data),
		vertex_data,
		GL_STATIC_DRAW
	);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	float uv_data[8] = {
		// point down left
		0.0f, 1.0f,
		// point up left
		0.0f, 0.0f,
		// point down right
		1.0f, 1.0f,
		// point up right
		1.0f, 0.0f
	};

	glGenBuffers(1, vbo_uv);
	glBindBuffer(GL_ARRAY_BUFFER, *vbo_uv);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(uv_data),
		uv_data,
		GL_STATIC_DRAW
	);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	return init_program("resources/glsl/circle.vert", "resources/glsl/circle.frag");
}

void init_textures(GLuint* tex, GLenum texnum, const char* path) {
	unsigned error;
	unsigned char* circle_data;
	unsigned circle_width, circle_height;

	error = lodepng_decode32_file(
		&circle_data, 
		&circle_width, 
		&circle_height,
		path
	);

	printf("%d\n", circle_width);
	printf("%d\n", circle_height);

	glGenTextures(1, tex);

	glActiveTexture(texnum);
	glBindTexture(GL_TEXTURE_2D, *tex);

	glTexImage2D(
		GL_TEXTURE_2D, 
		0, 
		GL_RGBA, 
		circle_width, 
		circle_height, 
		0,
		GL_RGBA, 
		GL_UNSIGNED_BYTE, 
		circle_data
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	free(circle_data);
}
