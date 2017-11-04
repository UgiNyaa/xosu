#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "gl-matrix.h"
#include "lodepng.h"
#include "circle_buffer.h"

#define WIDTH 1280
#define HEIGHT 720

GLuint init_program(const char* vs_path, const char* gs_path, const char* fs_path);

GLuint init_approach_vao(GLuint* vbo_pos, GLuint* vbo_uv);
GLuint init_circle_vao(GLuint* vbo_pos, GLuint* vbo_uv);

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

	//glFrontFace(GL_CCW);

	//GLuint approach_pos, approach_uv;
	//GLuint approach_vao = init_approach_vao(&approach_pos, &approach_uv);
	//GLuint approach_program = init_program("resources/glsl/approach.vert", "resources/glsl/approach.frag");

	//GLuint circle_pos, circle_uv;
	//GLuint circle_vao = init_circle_vao(&circle_pos, &circle_uv);
	GLuint circle_program = init_program("resources/glsl/circle.vert", "resources/glsl/circle.geom", "resources/glsl/circle.frag");
	GLuint approach_program = init_program("resources/glsl/approach.vert", "resources/glsl/approach.geom", "resources/glsl/approach.frag");

	GLuint circle_vao;
	glGenVertexArrays(1, &circle_vao);
	glBindVertexArray(circle_vao);

	GLuint circle_buf;
	glGenBuffers(1, &circle_buf);
	glBindBuffer(GL_ARRAY_BUFFER, circle_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(circle_buffer), circle_buffer, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	GLuint hitc_tex, appr_tex;
	init_textures(&hitc_tex, GL_TEXTURE0, "resources/textures/hitcircleoverlay@2x.png");
	init_textures(&hitc_tex, GL_TEXTURE1, "resources/textures/approachcircle@2x.png");

	GLuint circle_mvpID = glGetUniformLocation(circle_program, "MVP");
	GLuint circle_texID = glGetUniformLocation(circle_program, "tex");
	GLuint circle_timeID = glGetUniformLocation(circle_program, "time");

	GLuint approach_mvpID = glGetUniformLocation(approach_program, "MVP");
	GLuint approach_texID = glGetUniformLocation(approach_program, "tex");
	GLuint approach_timeID = glGetUniformLocation(approach_program, "time");

	int circle_off = 0;
	int circle_span = 0;
	float ar = 1;

	glfwSetTime(0.0);
	for (;;) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float projection[16];
		mat4_ortho(
			-WIDTH/2.0f, 
			WIDTH/2.0f, 
			-HEIGHT/2.0f, 
			HEIGHT/2.0f, 
			0.0f, 
			100000.0f, 
			projection
		);

		float eye[3] = { 0.0f, 0.0f, 0.0f };
		float center[3] = { 0.0f, 0.0f, 1.0f };
		float up[3] = { 0.0f, 1.0f, 0.0f };
		float view[16];
		mat4_lookAt(eye, center, up, view);

		float mvp[16];
		mat4_multiply(projection, view, mvp);

		glBindVertexArray(circle_vao);
		glEnableVertexAttribArray(0);

		// hit circle drawing
		{
			glUseProgram(circle_program);

			glUniform1i(circle_texID, 0);
			glUniformMatrix4fv(circle_mvpID, 1, GL_FALSE, mvp);
			glUniform1f(circle_timeID, (float)glfwGetTime());

			glDrawArrays(GL_POINTS, 0, CIRCLE_BUFFER_SIZE);
		}

		// approach circle drawing
		{
			glUseProgram(approach_program);
		
			glUniform1i(approach_texID, 1);
			glUniformMatrix4fv(approach_mvpID, 1, GL_FALSE, mvp);
			glUniform1f(approach_timeID, (float)glfwGetTime());

			glDrawArrays(GL_POINTS, 0, CIRCLE_BUFFER_SIZE);
		}

		glDisableVertexAttribArray(0);

		//glBindVertexArray(circle_vao);
		//glEnableVertexAttribArray(0);
		//glEnableVertexAttribArray(1);

		//// hit circle drawing
		//{
		//	glUseProgram(circle_program);

		//	glUniform1i(circle_texID, 0);
		//	glUniformMatrix4fv(circle_mvpID, 1, GL_FALSE, mvp);
		//	glUniform1f(circle_timeID, (float)glfwGetTime());

		//	glDrawArrays(GL_TRIANGLES, 0, CIRCLE_BUFFER_SIZE * 6);
		//}

		//// approach circle drawing
		//{
		//	glUseProgram(approach_program);
		//
		//	glUniform1i(approach_texID, 1);
		//	glUniformMatrix4fv(approach_mvpID, 1, GL_FALSE, mvp);
		//	glUniform1f(approach_timeID, (float)glfwGetTime());

		//	glDrawArrays(GL_TRIANGLES, 0, CIRCLE_BUFFER_SIZE * 6);
		//}

		//glDisableVertexAttribArray(0);
		//glDisableVertexAttribArray(1);

		//glUseProgram(approach_program);
		//glBindVertexArray(approach_vao);
		//glEnableVertexAttribArray(0);
		//glEnableVertexAttribArray(1);

		//glUniform1i(texID, 0);

		//float t = (float)glfwGetTime();
		//if (circle_buffer[circle_off + circle_span].t - ar < t)
		//	circle_span++;
		//else if (circle_buffer[circle_off].t + 0.2 < t) {
		//	circle_span--;
		//	circle_off++;
		//}

		//// circle drawing
		//for (int i = circle_off + circle_span - 1; i >= circle_off; i--) {
		//	float circle_model[16];
		//	float mvp[16];

		//	mat4_identity(circle_model);
		//	mat4_identity(mvp);

		//	float translation[3] = { 
		//		circle_buffer[i].x,
		//		circle_buffer[i].y,
		//		circle_buffer[i].t
		//	};
		//	mat4_translate(circle_model, translation, circle_model);

		//	mat4_identity(mvp);
		//	mat4_multiply(projection, view, mvp);
		//	mat4_multiply(mvp, circle_model, mvp);

		//	glUniform1i(texID, 0);
		//	glUniformMatrix4fv(mvpID, 1, GL_FALSE, mvp);

		//	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		//	if (circle_buffer[i].t > t)
		//	{
		//		float approach_model[16];
		//		mat4_identity(approach_model);

		//		translation[0] = 0.0f;
		//		translation[1] = 0.0f;
		//		translation[2] = -.1f;
		//		mat4_translate(circle_model, translation, circle_model);

		//		float scale[3] = {
		//			1.0f + 2.0f * (circle_buffer[i].t - t) / ar,
		//			1.0f + 2.0f * (circle_buffer[i].t - t) / ar,
		//			1.0f + 2.0f * (circle_buffer[i].t - t) / ar
		//		};
		//		mat4_scale(circle_model, scale, approach_model);

		//		mat4_identity(mvp);
		//		mat4_multiply(projection, view, mvp);
		//		mat4_multiply(mvp, approach_model, mvp);

		//		glUniform1i(texID, 1);
		//		glUniformMatrix4fv(mvpID, 1, GL_FALSE, mvp);

		//		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		//	}
		//}

		//glDisableVertexAttribArray(1);
		//glDisableVertexAttribArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

GLuint init_program(const char* vs_path, const char* gs_path, const char* fs_path) {
	GLint success = 0, logSize = 0;
	GLuint vsID = glCreateShader(GL_VERTEX_SHADER);
	GLuint gsID = glCreateShader(GL_GEOMETRY_SHADER);
	GLuint fsID = glCreateShader(GL_FRAGMENT_SHADER);

	FILE* fvs = fopen(vs_path, "rb");
	FILE* fgs = fopen(gs_path, "rb");
	FILE* ffs = fopen(fs_path, "rb");

	fseek(fvs, 0L, SEEK_END);
	fseek(fgs, 0L, SEEK_END);
	fseek(ffs, 0L, SEEK_END);

	int vs_size = ftell(fvs);
	int gs_size = ftell(fgs);
	int fs_size = ftell(ffs);

	fseek(fvs, 0L, SEEK_SET);
	fseek(fgs, 0L, SEEK_SET);
	fseek(ffs, 0L, SEEK_SET);

	char* vs_code = malloc(vs_size * sizeof(char) + 1);
	char* gs_code = malloc(gs_size * sizeof(char) + 1);
	char* fs_code = malloc(fs_size * sizeof(char) + 1);

	fread(vs_code, sizeof(char), vs_size, fvs);
	fread(gs_code, sizeof(char), gs_size, fgs);
	fread(fs_code, sizeof(char), fs_size, ffs);
	vs_code[vs_size] = '\0';
	gs_code[gs_size] = '\0';
	fs_code[fs_size] = '\0';

	fclose(fvs);
	fclose(fgs);
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

	glShaderSource(gsID, 1, &gs_code, &gs_size);
	glCompileShader(gsID);

	glGetShaderiv(gsID, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		glGetShaderiv(gsID, GL_INFO_LOG_LENGTH, &logSize);

		char* log = malloc(logSize);
		glGetShaderInfoLog(gsID, logSize, &logSize, log);

		printf("gs: \n");
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
	glAttachShader(pID, gsID);
	glAttachShader(pID, fsID);
	glLinkProgram(pID);

	free(vs_code);
	free(fs_code);

	return pID;
}

GLuint init_approach_vao(GLuint* vbo_pos, GLuint* vbo_uv) {
	GLuint vao;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	float vertex_data[8] = {
		// point down left
		-50.0f, -50.0f,
		// point up left
		-50.0f, +50.0f,
		// point down right
		+50.0f, -50.0f,
		// point up right
		+50.0f, +50.0f
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

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return vao;
}

GLuint init_circle_vao(GLuint* vbo_pos, GLuint* vbo_uv) {
	GLuint vao;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	float vertex_data[CIRCLE_BUFFER_SIZE * 6 * 3];
	float uv_data[CIRCLE_BUFFER_SIZE * 6 * 2];

	for (size_t i = 0; i < CIRCLE_BUFFER_SIZE; i++) {
		// counter clockwise

		// first triangle

		// point left down
		vertex_data[i * 6 * 3 + 0] = circle_buffer[i].x - 50.0f;
		vertex_data[i * 6 * 3 + 1] = circle_buffer[i].y - 50.0f;
		vertex_data[i * 6 * 3 + 2] = circle_buffer[i].t;
		uv_data[i * 6 * 2 + 0] = 0.0f;
		uv_data[i * 6 * 2 + 1] = 0.0f;

		// point right down
		vertex_data[i * 6 * 3 + 3] = circle_buffer[i].x + 50.0f;
		vertex_data[i * 6 * 3 + 4] = circle_buffer[i].y - 50.0f;
		vertex_data[i * 6 * 3 + 5] = circle_buffer[i].t;
		uv_data[i * 6 * 2 + 2] = 1.0f;
		uv_data[i * 6 * 2 + 3] = 0.0f;

		// point left up
		vertex_data[i * 6 * 3 + 6] = circle_buffer[i].x - 50.0f;
		vertex_data[i * 6 * 3 + 7] = circle_buffer[i].y + 50.0f;
		vertex_data[i * 6 * 3 + 8] = circle_buffer[i].t;
		uv_data[i * 6 * 2 + 4] = 0.0f;
		uv_data[i * 6 * 2 + 5] = 1.0f;

		// second triangle

		// point right down
		vertex_data[i * 6 * 3 + 9] = circle_buffer[i].x + 50.0f;
		vertex_data[i * 6 * 3 + 10] = circle_buffer[i].y - 50.0f;
		vertex_data[i * 6 * 3 + 11] = circle_buffer[i].t;
		uv_data[i * 6 * 2 + 6] = 1.0f;
		uv_data[i * 6 * 2 + 7] = 0.0f;

		// point right up
		vertex_data[i * 6 * 3 + 12] = circle_buffer[i].x + 50.0f;
		vertex_data[i * 6 * 3 + 13] = circle_buffer[i].y + 50.0f;
		vertex_data[i * 6 * 3 + 14] = circle_buffer[i].t;
		uv_data[i * 6 * 2 + 8] = 1.0f;
		uv_data[i * 6 * 2 + 9] = 1.0f;

		// point left up
		vertex_data[i * 6 * 3 + 15] = circle_buffer[i].x - 50.0f;
		vertex_data[i * 6 * 3 + 16] = circle_buffer[i].y + 50.0f;
		vertex_data[i * 6 * 3 + 17] = circle_buffer[i].t;
		uv_data[i * 6 * 2 + 10] = 0.0f;
		uv_data[i * 6 * 2 + 11] = 1.0f;
	}

	glGenBuffers(1, vbo_pos);
	glBindBuffer(GL_ARRAY_BUFFER, *vbo_pos);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(vertex_data),
		vertex_data,
		GL_STATIC_DRAW
	);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glGenBuffers(1, vbo_uv);
	glBindBuffer(GL_ARRAY_BUFFER, *vbo_uv);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(uv_data),
		uv_data,
		GL_STATIC_DRAW
	);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return vao;
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
