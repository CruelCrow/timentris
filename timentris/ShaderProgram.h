#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <memory>

#include "exceptions.h"
#include "OpenGL.h"
#include "Shader.h"

using namespace std;

class ShaderProgram {
private:
	GLuint _program;
public:
	ShaderProgram(void);
	~ShaderProgram(void);

	void attachShader(const Shader& s); //Attach shader to the shader program

	//Link created and loaded shader program
	bool link();
	//Check linked program validity
	bool validate();
	//Make shader program active
	void bind();
	//Make shader program inactive
	void unbind();

	GLint getAttribLocation(const GLchar *name) const;
	GLint getUniformLocation(const GLchar *name) const;

	//Same as attachShader, but no exceptions will be thrown
	friend ShaderProgram& operator<< (ShaderProgram &p, const Shader &s) throw();
	//Same as attachShader and can throw an exception (in case of wrong pointer)
	friend ShaderProgram* operator<< (ShaderProgram* p, const Shader &s);
	friend shared_ptr<ShaderProgram> operator<< (shared_ptr<ShaderProgram> p, const Shader &s);
};



/// <image url="$(ProjectDir)\comments\RenderingPipeline.png" scale="0.75" />