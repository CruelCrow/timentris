#pragma once
#include <memory>
#include <atomic>
#include <mutex>
#include <chrono>
#include <future>
#include <string>
#include <iostream>
#include <armadillo>

#include "GLWindow.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "TimentrisGame.h"
#include "Configuration.h"
#include "Input.h"
#include "Message.h"
#include "TransformationMatrices.h"
#include "Camera.h"
#include "exceptions.h"
#include "CircularReferenceFix.h"

namespace TimentrisMessages {
	namespace RenderMessages {
		class RenderMessage : public TimentrisMessages::Message {};

		class Zoom : public RenderMessage {
		public:
			Zoom(POINT mouseScreenCoordinates, float zoom) : mouseScreenCoordinates(mouseScreenCoordinates), zoom(zoom) {};
			POINT mouseScreenCoordinates;
			float zoom;
		};

		class Move : public RenderMessage {
		public:
			Move(float x, float y) : x(x), y(y) {};
			float x;
			float y;
		};

		class Rotate : public RenderMessage {
		public:
			Rotate(float x, float y) : x(x), y(y) {};
			float x;
			float y;
		};

		class ResetView : public RenderMessage {};

		//TODO: ?Move: Left-Right, Up-Down
		//TODO: Animation messages

		class Animate : public RenderMessage {};
	}
}

using namespace std;
using namespace arma;
using namespace TimentrisMessages;
using namespace TimentrisMessages::RenderMessages;

class Render {
	class GridCube { //TODO: should it be struct?
	public:
		static const GLfloat _cubeVertices[];
		static const GLuint _cubeIndices[];
		static const GLfloat _cubeTriangles[];
		static const GLubyte _cubeSides[];
		static const GLfloat _cubeLines[];
	private:
		shared_ptr<ShaderProgram> _shaderProgram;
		fmat44 _V;
		fmat44 _MV;
		fmat44 _MVP;
		gridCell _type; //piece, brick or empty
	public:
		GridCube() {};
		GridCube(const shared_ptr<ShaderProgram> shaderProgram, const fmat44& v, const fmat44& mv, const fmat44& mvp, const gridCell type) : _shaderProgram(shaderProgram), _V(v), _MV(mv), _MVP(mvp), _type(type) {};

		void draw();

		//for sorting (far should be first)
		bool operator<(const GridCube &other) const;
	};
private:
	GLWindowPtr _windowPtr;
	TimentrisGamePtr _gamePtr;
	InputPtr _inputPtr;

	atomic<bool> _running;

	ShaderProgram _shaderProgram;
	shared_ptr<ShaderProgram> _gridShaderProgram;

	CameraPerspective _camera;
public:
	Render();
	Render(GLWindowPtr window, TimentrisGamePtr game);
	Render(const Render& other); //disallow copy constructor
	Render& operator=(const Render&); //disallow assignment operator
	~Render(void);

	void attachWindow(GLWindowPtr windowPtr);
	void attachGame(TimentrisGamePtr gamePtr);
	void attachInput(InputPtr inputPtr);

	void run();
	bool isRunning() const;

private:
	void init(); //OpenGL initialization

	void mainLoop();
	double getTimerTicks();
	
	array<vector<GridCube>, 4> _cubes;
	void render();

public:
	//TODO: implement << operators for messages
	//TODO: frame mutex
	friend Render& operator<< (Render &g, const Zoom &m);
	friend Render& operator<< (Render &g, const RenderMessages::Move &m);
	friend Render& operator<< (Render &g, const RenderMessages::Rotate &m);
	friend Render& operator<< (Render &g, const RenderMessages::ResetView &m);
};
typedef shared_ptr<Render> RenderPtr;
//TODO: menu
//TODO: HUD

//The camera in OpenGL cannot move and is defined to be located at (0,0,0) facing the negative Z direction.
//Rotation matrix:
/// <image url="$(ProjectDir)\comments\gl_anglestoaxes01.png" scale="0.75" />