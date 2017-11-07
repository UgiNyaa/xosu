#ifndef LINEAR_CIRCLE_BUFFER_H
#define LINEAR_CIRCLE_BUFFER_H

typedef struct {
	struct {
		float x;
		float y;
		float t;
	} begin;
	struct {
		float x;
		float y;
		float t;
	} end;
	
} LinearSlider;

#define LINEAR_SLIDER_BUFFER_SIZE 1 * 3 * 2

static const float linear_slider_buffer[LINEAR_SLIDER_BUFFER_SIZE] = {
	200.0f, 100.0f, 2.0f,
	-200.0f, 100.0f, 3.0f
};

#endif
