#include "ShaderProgram.h"


ShaderProgram::ShaderProgram(void) {
	//Create shader program
	if ((_program = glCreateProgram()) == 0)	{
		cerr << "Creating shader program fail " << glGetError() << endl;
		throw new OpenGLExceptions::CreatingShaderFailed();
		return;
	}
}


ShaderProgram::~ShaderProgram(void) {
	unbind(); //Make current shader program inactive
	glDeleteProgram(_program);
}


void ShaderProgram::attachShader(const Shader& s) {
	//Attach loaded shaders to the shader program
	glAttachShader(_program, s._shader);

	OPENGL_CHECK_FOR_ERRORS();

	if (glGetError() != GL_NO_ERROR) {
		glDeleteProgram(_program);
		throw new OpenGLExceptions::AttachingShaderFailed();
		return;
	}

	return;
}

bool ShaderProgram::link() {
	glLinkProgram(_program);
	GLint status;
	glGetProgramiv(_program, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		GLint length;
		GLchar buffer[1024];
		glGetProgramInfoLog(_program, 1024, &length, buffer);
		cerr << "Shader program link faild: " << buffer << endl;
		throw new OpenGLExceptions::LinkingShaderProgramFailed();
		return false;
	}
	validate();
	return true;
}

bool ShaderProgram::validate() {
	glValidateProgram(_program);
	GLint status;
	glGetProgramiv(_program, GL_VALIDATE_STATUS, &status);
	if (status != GL_TRUE) {
		GLint length;
		GLchar buffer[1024];
		glGetProgramInfoLog(_program, 1024, &length, buffer);
		cerr << "Shader program invalid: " << buffer << endl;
		throw new OpenGLExceptions::ShaderProgramInvalid();
		return false;
	}
	return true;
}

void ShaderProgram::bind() {
	//Let's make this shader program active
	glUseProgram(_program);
	validate();
}

void ShaderProgram::unbind() {
	//Let's make this shader program active
	glUseProgram(0);
}

GLint ShaderProgram::getAttribLocation(const GLchar *name) const {
	return glGetAttribLocation(_program, name);
}

GLint ShaderProgram::getUniformLocation(const GLchar *name) const {
	return glGetUniformLocation(_program, name);
}



ShaderProgram& operator<< (ShaderProgram &p, const Shader &s) throw() {
	try {
		p.attachShader(s);
	} catch (OpenGLExceptions::AttachingShaderFailed) {
	}
	return p;
}
ShaderProgram* operator<< (ShaderProgram* p, const Shader &s) {
	try {
		p->attachShader(s);
	} catch (OpenGLExceptions::AttachingShaderFailed) {
	}
	return p;
}
shared_ptr<ShaderProgram> operator<< (shared_ptr<ShaderProgram> p, const Shader &s) {
	try {
		p->attachShader(s);
	} catch (OpenGLExceptions::AttachingShaderFailed) {
	}
	return p;
}