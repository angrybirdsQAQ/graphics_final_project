#ifndef fps_counter_h
#define fps_counter_h

#include "../Externals/Include/Include.h"

class FPS_Counter {
public:
	FPS_Counter();
	void get_start_frequency();
	void get_end_frequency();
	double get_frametime();

private:
	LARGE_INTEGER frequenz, count_start, count_ende;
};

#endif /*fps_counter_h*/