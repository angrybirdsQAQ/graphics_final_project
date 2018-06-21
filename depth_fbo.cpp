#include "depth_fbo.h"

// configure depth fbo for shadow mapping
void configure_depth_fbo(int width, int height) {
	glGenFramebuffers(1, &depth_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);

	glGenTextures(1, &depth_tex);
	glBindTexture(GL_TEXTURE_2D, depth_tex);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F,
				 width, height, 0,
				 GL_DEPTH_COMPONENT32F, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_tex, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);	//unbind
}

void reset_depth_fbo(int width, int height)
{
	glDeleteFramebuffers(1, &depth_fbo);
	glDeleteTextures(1, &depth_tex);

	configure_depth_fbo(width, height);
}