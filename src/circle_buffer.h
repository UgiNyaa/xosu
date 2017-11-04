#ifndef CIRCLE_BUFFER_H
#define CIRCLE_BUFFER_H

typedef struct {
	float x;
	float y;
	float t;
} Circle;

#define CIRCLE_BUFFER_SIZE 3

static const Circle circle_buffer[CIRCLE_BUFFER_SIZE] = {
	{ 96.0f, 96.0f, 2.225f },
	{128.0f, 352.0f, 2.525f},
	{ 224.0f, 192.0f, 2.675f }
};

#endif