#include "OpenGL.h"

void OpenGLPrintDebugInfo()
{
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	std::cout << "OpenGL render context information:\n"
		 << "  Renderer       : " << (const char*)glGetString(GL_RENDERER) << "\n"
		 << "  Vendor         : " << (const char*)glGetString(GL_VENDOR) << "\n"
		 << "  Version        : " << (const char*)glGetString(GL_VERSION) << "\n"
		 << "  GLSL version   : " << (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n"
		 << "  OpenGL version : " << major << "." << minor << "\n" << std::endl;

	std::cout << "GL_MAX_VERTEX_ATTRIBS = " << GL_MAX_VERTEX_ATTRIBS << std::endl;
	std::cout << "GL_MAX_TEXTURE_IMAGE_UNITS = " << GL_MAX_TEXTURE_IMAGE_UNITS << std::endl;

	OPENGL_CHECK_FOR_ERRORS();
}

bool OpenGLInitExtensions()
{
	// Frame buffer
	OPENGL_GET_PROC(PFNWGLSWAPINTERVALFARPROC, wglSwapIntervalEXT);
	// Texture
	OPENGL_GET_PROC(PFNGLACTIVETEXTUREPROC, glActiveTexture);
	// VAO
	OPENGL_GET_PROC(PFNGLGENVERTEXARRAYSPROC,    glGenVertexArrays);
	OPENGL_GET_PROC(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays);
	OPENGL_GET_PROC(PFNGLBINDVERTEXARRAYPROC,    glBindVertexArray);
	// VBO
	OPENGL_GET_PROC(PFNGLGENBUFFERSPROC,    glGenBuffers);
	OPENGL_GET_PROC(PFNGLDELETEBUFFERSPROC, glDeleteBuffers);
	OPENGL_GET_PROC(PFNGLBINDBUFFERPROC,    glBindBuffer);
	OPENGL_GET_PROC(PFNGLBUFFERDATAPROC,    glBufferData);
	OPENGL_GET_PROC(PFNGLBUFFERSUBDATAPROC, glBufferSubData);
	// Shaders
	OPENGL_GET_PROC(PFNGLCREATEPROGRAMPROC,     glCreateProgram);
	OPENGL_GET_PROC(PFNGLDELETEPROGRAMPROC,     glDeleteProgram);
	OPENGL_GET_PROC(PFNGLLINKPROGRAMPROC,       glLinkProgram);
	OPENGL_GET_PROC(PFNGLVALIDATEPROGRAMPROC,   glValidateProgram);
	OPENGL_GET_PROC(PFNGLUSEPROGRAMPROC,        glUseProgram);
	OPENGL_GET_PROC(PFNGLGETPROGRAMIVPROC,      glGetProgramiv);
	OPENGL_GET_PROC(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog);
	OPENGL_GET_PROC(PFNGLCREATESHADERPROC,      glCreateShader);
	OPENGL_GET_PROC(PFNGLDELETESHADERPROC,      glDeleteShader);
	OPENGL_GET_PROC(PFNGLSHADERSOURCEPROC,      glShaderSource);
	OPENGL_GET_PROC(PFNGLCOMPILESHADERPROC,     glCompileShader);
	OPENGL_GET_PROC(PFNGLATTACHSHADERPROC,      glAttachShader);
	OPENGL_GET_PROC(PFNGLDETACHSHADERPROC,      glDetachShader);
	OPENGL_GET_PROC(PFNGLGETSHADERIVPROC,       glGetShaderiv);
	OPENGL_GET_PROC(PFNGLGETSHADERINFOLOGPROC,  glGetShaderInfoLog);
	// Shaders attributes
	OPENGL_GET_PROC(PFNGLGETATTRIBLOCATIONPROC,        glGetAttribLocation);
	OPENGL_GET_PROC(PFNGLVERTEXATTRIBPOINTERPROC,      glVertexAttribPointer);
	OPENGL_GET_PROC(PFNGLENABLEVERTEXATTRIBARRAYPROC,  glEnableVertexAttribArray);
	OPENGL_GET_PROC(PFNGLDISABLEVERTEXATTRIBARRAYPROC, glDisableVertexAttribArray);
	// Shaders uniforms
	OPENGL_GET_PROC(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation);
	OPENGL_GET_PROC(PFNGLUNIFORMMATRIX4FVPROC,   glUniformMatrix4fv);
	OPENGL_GET_PROC(PFNGLUNIFORM1IPROC,          glUniform1i);
	OPENGL_GET_PROC(PFNGLUNIFORM1FPROC,          glUniform1f);
	OPENGL_GET_PROC(PFNGLUNIFORM3FPROC,          glUniform3f);
	OPENGL_GET_PROC(PFNGLUNIFORM4FPROC,          glUniform4f);

	OPENGL_CHECK_FOR_ERRORS();

	return true;
}

//Declare OpenGL extensions
// Frame buffer
PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = NULL;
// Texture
PFNGLACTIVETEXTUREPROC glActiveTexture = NULL;
// VAO
PFNGLGENVERTEXARRAYSPROC    glGenVertexArrays    = NULL;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = NULL;
PFNGLBINDVERTEXARRAYPROC    glBindVertexArray    = NULL;
// VBO
PFNGLGENBUFFERSPROC    glGenBuffers    = NULL;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = NULL;
PFNGLBINDBUFFERPROC    glBindBuffer    = NULL;
PFNGLBUFFERDATAPROC    glBufferData    = NULL;
PFNGLBUFFERSUBDATAPROC glBufferSubData = NULL;
// Shaders
PFNGLCREATEPROGRAMPROC     glCreateProgram     = NULL;
PFNGLDELETEPROGRAMPROC     glDeleteProgram     = NULL;
PFNGLLINKPROGRAMPROC       glLinkProgram       = NULL;
PFNGLVALIDATEPROGRAMPROC   glValidateProgram   = NULL;
PFNGLUSEPROGRAMPROC        glUseProgram        = NULL;
PFNGLGETPROGRAMIVPROC      glGetProgramiv      = NULL;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = NULL;
PFNGLCREATESHADERPROC      glCreateShader      = NULL;
PFNGLDELETESHADERPROC      glDeleteShader      = NULL;
PFNGLSHADERSOURCEPROC      glShaderSource      = NULL;
PFNGLCOMPILESHADERPROC     glCompileShader     = NULL;
PFNGLATTACHSHADERPROC      glAttachShader      = NULL;
PFNGLDETACHSHADERPROC      glDetachShader      = NULL;
PFNGLGETSHADERIVPROC       glGetShaderiv       = NULL;
PFNGLGETSHADERINFOLOGPROC  glGetShaderInfoLog  = NULL;
// Shaders attributes
PFNGLGETATTRIBLOCATIONPROC        glGetAttribLocation        = NULL;
PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer      = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray  = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = NULL;
// Shaders uniforms
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = NULL;
PFNGLUNIFORMMATRIX4FVPROC   glUniformMatrix4fv   = NULL;
PFNGLUNIFORM1IPROC          glUniform1i          = NULL;
PFNGLUNIFORM1FPROC          glUniform1f          = NULL;
PFNGLUNIFORM3FPROC          glUniform3f          = NULL;
PFNGLUNIFORM4FPROC          glUniform4f          = NULL;