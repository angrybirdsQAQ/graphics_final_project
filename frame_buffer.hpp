//
//  frame_buffer.hpp
//  AS3
//


#ifndef frame_buffer_hpp
#define frame_buffer_hpp

#include "../Externals/Include/Include.h"

typedef struct _frame_buffer{
    GLuint FBO;
    GLuint depthRBO;
    GLuint FBODataTexture;
    
    
    _frame_buffer();   // constructor
    void resetFBO(int width, int height);   // reset fbo when screen size changes
    void bind_and_clear_draw_buffer();
    void unbind_draw_buffer();
    void bind_fbo_texture();
    void FBO_2_PPM_file(int width, int height, char *file_path);    // debug
    
} FrameBuffer;


///////////////////////// Functions ////////////////////////////

void FBO_2_PPM_file(int width, int height, char *file_path);



#endif /* frame_buffer_hpp */
