#ifndef CIRCLE_BUFFER_H
#define CIRCLE_BUFFER_H

typedef struct {
	float x;
	float y;
	float t;
} Circle;

#define CIRCLE_BUFFER_SIZE 3

static const Circle circle_buffer[CIRCLE_BUFFER_SIZE] = {
	{ 200.0f, 0.0f, 2.0f },
	{ 0.0f, 0.0f, 2.5f},
	{ -200.0f, 0.0f, 3.0f }
};

#endif