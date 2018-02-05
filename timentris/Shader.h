#pragma once
#include <string>
#include <iostream>
#include <fstream>

#include "exceptions.h"
#include "OpenGL.h"

using namespace std;

class Shader {
	friend class ShaderProgram;
private:
	GLuint _shader;
public:
	Shader(const string& fileName, int type);
	~Shader(void);
};

class VertexShader : public Shader {
public:
	VertexShader(const string& fileName) : Shader(fileName, GL_VERTEX_SHADER) {};
};

class FragmentShader : public Shader {
public:
	FragmentShader(const string& fileName) : Shader(fileName, GL_FRAGMENT_SHADER) {};
};

class GeometryShader : public Shader {
public:
	GeometryShader(const string& fileName) : Shader(fileName, GL_GEOMETRY_SHADER) {};
};