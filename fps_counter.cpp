#include "fps_counter.h"

FPS_Counter::FPS_Counter()
{
	QueryPerformanceCounter(&frequenz);
}

void FPS_Counter::get_start_frequency()
{
	QueryPerformanceCounter(&count_start);
	//std::cout << count_start.QuadPart << std::endl;
}

void FPS_Counter::get_end_frequency()
{
	QueryPerformanceCounter(&count_ende);
	//std::cout << count_ende.QuadPart << std::endl;
}

double FPS_Counter::get_frametime()
{
	double t_start = (double) count_start.QuadPart;
	double t_end = (double) count_ende.QuadPart;
	double ms_per_frame = (t_end - t_start) / (double)frequenz.QuadPart;
	double fps = 1.0 / (ms_per_frame / 1000.0);
	return ms_per_frame;
}