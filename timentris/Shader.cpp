#include "Shader.h"


Shader::Shader(const string& fileName, int type) {
	string debugShaderType;
	switch (type) {
	case GL_VERTEX_SHADER:
		debugShaderType = "vertex";
		break;
	case GL_FRAGMENT_SHADER:
		debugShaderType = "fragment";
		break;
	case GL_GEOMETRY_SHADER:
		debugShaderType = "geometry";
		break;
	default:
		throw new OpenGLExceptions::WrongShaderType();
		return;
		break;
	}

	string  shaderSource; //a source code of our shader
	unsigned __int32 sourceLength;

	if ((_shader = glCreateShader(type)) == 0) {
		cerr << "Creating " << debugShaderType << " shader program fail " << glGetError() << endl;
		throw new OpenGLExceptions::CreatingShaderFailed();
		return;
	}

	//Load shader
	ifstream* streamSource = new ifstream(fileName);
	if (!streamSource->is_open()) {
		cerr << "Can't load " << debugShaderType << " shader souce file " << glGetError() << endl;
		throw new OpenGLExceptions::CreatingShaderFailed();
		return;
	}
	shaderSource = string((std::istreambuf_iterator<char>(*streamSource)), 
		std::istreambuf_iterator<char>());
	delete streamSource;
	sourceLength = shaderSource.length();
	if (sourceLength <= 0) {
		cerr << "Can't load " << debugShaderType << " shader souce file " << glGetError() << endl;
		glDeleteShader(_shader);
		throw new OpenGLExceptions::CreatingShaderFailed();
		return;
	}

	//Assign the source code to the shader and compile it
	const char* c_shaderSource = shaderSource.c_str();
	glShaderSource(_shader, 1, (const GLchar**)&c_shaderSource, (const GLint*)&sourceLength);
	glCompileShader(_shader);

	//Validate shader compilation status
	GLint status;
	glGetShaderiv(_shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		GLint length;
		GLchar buffer[1024];
		glGetShaderInfoLog(_shader, 1024, &length, buffer);
		cerr << debugShaderType << " shader compilation faild: " << buffer << endl;
		glDeleteShader(_shader);
		throw new OpenGLExceptions::CompilingShaderFailed();
		return;
	}
}


Shader::~Shader(void) {
	glDeleteShader(_shader);
}
