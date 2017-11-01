#ifndef CIRCLE_BUFFER_H
#define CIRCLE_BUFFER_H

typedef struct {
	unsigned int t;
	float x;
	float y;
} Circle;

static Circle circle_buffer[] = {
	{ 5000, 0.0f, 0.0f },
	{ 5500, 5.0f, 5.0f },
	{ 160000, -5.0f, -5.0f }
};

#endif