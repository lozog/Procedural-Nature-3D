/**
 * GlErrorCheck
 */

#pragma once

#include <string>

// Helper Macros
#if(DEBUG)
	#define CHECK_GL_ERRORS checkGLErrors(__FILE__, __LINE__)
	#define CHECK_FRAMEBUFFER_COMPLETENESS checkFramebufferCompleteness()
#else
	#define CHECK_GL_ERRORS
	#define CHECK_FRAMEBUFFER_COMPLETENESS
#endif


void checkGLErrors(const std::string & currentFileName, int currentLineNumber);

void checkFramebufferCompleteness();

