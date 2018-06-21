//
//  frame_buffer.cpp
//  AS3
//

#include "frame_buffer.hpp"

static const GLfloat sky_blue[] = {0.843f, 0.913f, 0.988f, 1.0f};
static const GLfloat one = 1.0f;



_frame_buffer::_frame_buffer(){
    glGenFramebuffers(1, &FBO);
}

void _frame_buffer::resetFBO(int width, int height){
    // reset & bind
    glDeleteRenderbuffers(1, &depthRBO);
    glDeleteTextures(1, &FBODataTexture);
    glGenRenderbuffers(1, &depthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
    
    
    // generate a texture for FBO to write in
    glGenTextures(1, &FBODataTexture);
    glBindTexture(GL_TEXTURE_2D, FBODataTexture);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    
    // bind the frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO);
    
    // attach a texture image to a framebuffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBODataTexture, 0);
}

void _frame_buffer::bind_and_clear_draw_buffer(){
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    // Clear the color buffer (GL_COLOR) with the color of white
    // Clear the depth buffer (GL_DEPTH) with value one
    glClearBufferfv(GL_COLOR, 0, sky_blue);
    glClearBufferfv(GL_DEPTH, 0, &one);
}

void _frame_buffer::unbind_draw_buffer(){
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void _frame_buffer::bind_fbo_texture(){
    glBindTexture(GL_TEXTURE_2D, FBODataTexture);   
}


// for debug usage
// code taken from https://stackoverflow.com/questions/31254444/how-to-save-a-texture-as-an-image-file-using-libraries-related-to-opengl/31800675
void _frame_buffer::FBO_2_PPM_file(int width, int height, char *file_path)
{
    FILE    *output_image;
    int     output_width, output_height;
    
    output_width = width;
    output_height = height;
    
    /// READ THE PIXELS VALUES from FBO AND SAVE TO A .PPM FILE
    int             i, j, k;
    unsigned char   *pixels = (unsigned char*)malloc(output_width*output_height*3);
    
    /// READ THE CONTENT FROM THE FBO
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(0, 0, output_width, output_height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    
    output_image = fopen(file_path, "wt");
    fprintf(output_image,"P3\n");
    fprintf(output_image,"# Created by Ricao\n");
    fprintf(output_image,"%d %d\n",output_width,output_height);
    fprintf(output_image,"255\n");
    
    k = 0;
    for(i=0; i<output_width; i++)
    {
        for(j=0; j<output_height; j++)
        {
            fprintf(output_image,"%u %u %u ",(unsigned int)pixels[k],(unsigned int)pixels[k+1],
                    (unsigned int)pixels[k+2]);
            k = k+3;
        }
        fprintf(output_image,"\n");
    }
    free(pixels);
}


