#ifndef depth_fbo_h
#define depth_fbo_h

#include "../Externals/Include/Include.h"

extern GLuint depth_fbo, depth_tex;

void configure_depth_fbo(int width, int height);
void reset_depth_fbo(int width, int height);

#endif	/*depth_fbo_h*/
