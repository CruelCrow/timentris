#include "Render.h"

Render::Render() : _running(false) {
}

Render::Render(GLWindowPtr window, TimentrisGamePtr game) : _running(false) {
	_windowPtr = window;
	_gamePtr = game;

	run();
}

Render::~Render(void) {
	//Make current VBO inactive
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//TODO?: remove VBO
	//Make current VAO inactive
	glBindVertexArray(0);
	//TODO?: remove VAO
	//TODO?: remove shader porgrams and textures
}

void Render::attachWindow(GLWindowPtr windowPtr) {
	if (isRunning()) {
		throw new RenderExceptions::AlreadyRunning();
		return;
	}
	_windowPtr = windowPtr;
}

void Render::attachGame(TimentrisGamePtr gamePtr) {
	if (isRunning()) {
		throw new RenderExceptions::AlreadyRunning();
		return;
	}
	_gamePtr = gamePtr;
}

void Render::attachInput(InputPtr inputPtr) {
	if (isRunning()) {
		throw new RenderExceptions::AlreadyRunning();
		return;
	}
	_inputPtr = inputPtr;
}

void Render::run() {
	if (isRunning()) {
		throw new RenderExceptions::AlreadyRunning();
		return;
	}
	if (_windowPtr == NULL) {
		throw new RenderExceptions::NoWindowAttached();
		return;
	}
	if (_gamePtr == NULL) {
		throw new RenderExceptions::NoGameAttached();
		return;
	}
	if (_inputPtr == NULL) {
		throw new RenderExceptions::NoInputAttached();
		return;
	}
	init();
	mainLoop();
}

bool Render::isRunning() const {
	return (bool)_running;
}

void Render::mainLoop() {
	MSG    msg;
	double beginFrameTime, deltaTime;
	unsigned long long renderedFrames = 0;

	_running = true;

	//wait until Game is initialized
	while (
		_gamePtr == NULL || !_gamePtr->isRunning()
		||
		_inputPtr == NULL || !_inputPtr->isRunning()
		)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

	while (_windowPtr->_running)
	{
		//Handle messages from the messages queue
		while (PeekMessage(&msg, _windowPtr->_hWnd, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) { //TODO: message from Input object should be sent to this Render object
				_windowPtr->_running = false;
				break;
			}

			//Send message to Input
			*_inputPtr << msg;

			//TranslateMessage(&msg); //We DON'T need to translate keys to chars
			DispatchMessage(&msg);
		}

		//GLWindowInput(&g_window);

		//If the window is running and active (the focus is on window)
		if (_windowPtr->_running && _windowPtr->_active)
		{
			//take time from the timer
			beginFrameTime = getTimerTicks();

			render(); //for test only
			SwapBuffers(_windowPtr->_hDC);

			//calculate timer delta
			deltaTime = getTimerTicks() - beginFrameTime;

			//GLWindowUpdate(&g_window, deltaTime);

			renderedFrames++; //count rendered frames (just in case)
		}
	}

	_running = false;
}

double Render::getTimerTicks() {
	QueryPerformanceCounter(&_windowPtr->_qpc);
	return _windowPtr->_timerFrequency * _windowPtr->_qpc.QuadPart;
}

void Render::init() {
	//Viewport to the hole window
	glViewport(0, 0, _windowPtr->_width, _windowPtr->_height);

	//Disable/enable vsync
	if (wglSwapIntervalEXT) wglSwapIntervalEXT(1); //0=disable, 1=60fps limit

	//OpenGL parameters
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);	
	glDepthFunc(GL_LESS); //Accept fragment if it closer to the camera than the former one
	//glEnable(GL_CULL_FACE); //disable for test - what if we want to see the inside of the cubes?

	//For test - opacity
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //default

	//Draw as wireframe?
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	_shaderProgram << VertexShader("shaders\\lesson.vs.glsl") << FragmentShader("shaders\\lesson.fs.glsl");
	_shaderProgram.link(); //Link shader program
	//_shaderProgram.bind(); //Make shader program active
	_shaderProgram.validate();

	_gridShaderProgram = shared_ptr<ShaderProgram>(new ShaderProgram());
	_gridShaderProgram << VertexShader("shaders\\grid.vs.glsl")/* << GeometryShader("shaders\\grid.gs.glsl")*/ << FragmentShader("shaders\\grid.fs.glsl");
	_gridShaderProgram->link(); //Link shader program
	_gridShaderProgram->bind(); //Make shader program active
	_gridShaderProgram->validate();

	//Why?
	//
	GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
	//
	
	_camera.lock();
	_camera.zoomLimitsSet(Camera::ZoomLimit(0.25f, 2.5f));
	_camera.positionSetDefault(Camera::Coordinates(0,0,0)); _camera.positionReset();
	_camera.headRotationSetDefault(Camera::Angles(0,0,0)); _camera.headRotationReset();
	_camera.circlingAroundSetDefault(Camera::Angles(0,0,0)); _camera.circlingAroundReset();
	_camera.circlingAroundLimitsSet(Camera::AnglesLimit(Camera::Angles(-90, -90, -360), Camera::Angles(90, 90, 360)));
	_camera.circlingAroundBasisLimitsSet(Camera::CoordinatesLimit(Camera::Coordinates(-50.0f, -50.0f, -1000.0f), Camera::Coordinates(50.0f, 50.0, 1000.0f)));
	_camera.circlingDistanceSetDefault(Camera::Coordinates(0,0,-50.0f)); _camera.circlingDistanceReset();
	_camera.unlock();

	//check for errors
	OPENGL_CHECK_FOR_ERRORS();
}

void Render::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	static float tmp = 0;

	fmat44 projectionMatrix;
	fmat44 viewMatrix;
	fmat44 viewProjectionMatrix;
	_camera.lock();
	projectionMatrix = _camera.getProjectionMatrix();
	viewMatrix = _camera.getViewMatrix();
	viewProjectionMatrix = _camera.getViewProjectionMatrix();
	_camera.unlock();

	//Make grid shader program active
	_gridShaderProgram->bind(); //Make shader program active

	//draw axes and game field
	_gamePtr->getCupPtr()->lock();
	array<Cup3dSubspace, 4> gameFieldSubspaces = _gamePtr->getCupPtr()->get3dOrtonormalSubspaces();
	_gamePtr->getCupPtr()->unlock();


	/*GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, 24*sizeof(GLfloat), GridCube::_cubeVertices, GL_STATIC_DRAW);

	GLuint indexbuffer;
	glGenBuffers(1, &indexbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36*sizeof(GLuint), GridCube::_cubeIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(_gridShaderProgram->getAttribLocation("vPosition"));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
            _gridShaderProgram->getAttribLocation("vPosition"),
            3,			//size
            GL_FLOAT,	//type
            GL_FALSE,	//normalized?
            0,			//stride
            (void*)0	//array buffer offset
    );
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);*/

	GLuint trianglesbuffer;
	glGenBuffers(1, &trianglesbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, trianglesbuffer);
	glBufferData(GL_ARRAY_BUFFER, 36*3*sizeof(GLfloat), GridCube::_cubeTriangles, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(_gridShaderProgram->getAttribLocation("vPosition"));
    glBindBuffer(GL_ARRAY_BUFFER, trianglesbuffer);
    glVertexAttribPointer(
            _gridShaderProgram->getAttribLocation("vPosition"),
            3,				//size
            GL_FLOAT,		//type
            GL_FALSE,		//normalized?
            0,				//stride
            (void*)0		//array buffer offset
    );

	GLuint linebuffer;
	glGenBuffers(1, &linebuffer);
	glBindBuffer(GL_ARRAY_BUFFER, linebuffer);
	glBufferData(GL_ARRAY_BUFFER, 24*3*sizeof(GLfloat), GridCube::_cubeLines, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(_gridShaderProgram->getAttribLocation("vLinePosition"));
    glBindBuffer(GL_ARRAY_BUFFER, linebuffer);
    glVertexAttribPointer(
            _gridShaderProgram->getAttribLocation("vLinePosition"),
            3,				//size
            GL_FLOAT,		//type
            GL_FALSE,		//normalized?
            0,				//stride
            (void*)0		//array buffer offset
    );

	/*GLuint sidesbuffer;
	glGenBuffers(1, &sidesbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, sidesbuffer);
	glBufferData(GL_ARRAY_BUFFER, 36*sizeof(GLfloat), GridCube::_cubeSides, GL_STATIC_DRAW);
	
	//cout << _gridShaderProgram->getAttribLocation("vSide") << endl;
	glEnableVertexAttribArray(_gridShaderProgram->getAttribLocation("vSide"));
    glBindBuffer(GL_ARRAY_BUFFER, sidesbuffer);
    glVertexAttribPointer(
            _gridShaderProgram->getAttribLocation("vSide"),
            1,				//size
			GL_UNSIGNED_BYTE,//type
            GL_FALSE,		//normalized?
            0,				//stride
            (void*)0		//array buffer offset
    );*/

	//generate cubes
	vector<GridCube> cubes;
	
	future<void> futXyz = async([&]() {
		const Cube<gridCell>& grid = gameFieldSubspaces[SUBSPACE_XYZ].getGrid();
		unsigned int sizeX = gameFieldSubspaces[SUBSPACE_XYZ].getSizeX();
		unsigned int sizeY = gameFieldSubspaces[SUBSPACE_XYZ].getSizeY();
		unsigned int sizeZ = gameFieldSubspaces[SUBSPACE_XYZ].getSizeZ();
		if (_cubes[SUBSPACE_XYZ].size() == 0) {
			_cubes[SUBSPACE_XYZ].reserve(grid.n_elem);
			_cubes[SUBSPACE_XYZ].resize(grid.n_elem);
		}
		for (int x = 0; x < sizeX; x++) {
			for (int y = 0; y < sizeY; y++) {
				for (int z = 0; z < sizeZ; z++) {
					fmat44 modelMatrix = TransformationMatrices::getTranslationMatrix(
						y - (float)sizeY/2 - 25,
						z - (float)sizeZ/2,
						x - (float)sizeX/2
					);
					fmat44 MVP = viewProjectionMatrix * modelMatrix;
					fmat44 MV = viewMatrix * modelMatrix;
					_cubes[SUBSPACE_XYZ][sizeY*sizeZ*x + sizeZ*y + z] = GridCube(_gridShaderProgram, viewMatrix, MV, MVP, grid(x,y,z));
				}
			}
		}
		sort(_cubes[SUBSPACE_XYZ].begin(), _cubes[SUBSPACE_XYZ].end());
	});
	
	future<void> futXyw = async([&]() {
		const Cube<gridCell>& grid = gameFieldSubspaces[SUBSPACE_XYW].getGrid();
		unsigned int sizeX = gameFieldSubspaces[SUBSPACE_XYW].getSizeX();
		unsigned int sizeY = gameFieldSubspaces[SUBSPACE_XYW].getSizeY();
		unsigned int sizeZ = gameFieldSubspaces[SUBSPACE_XYW].getSizeZ();
		if (_cubes[SUBSPACE_XYW].size() == 0) {
			_cubes[SUBSPACE_XYW].reserve(grid.n_elem);
			_cubes[SUBSPACE_XYW].resize(grid.n_elem);
		}
		for (int x = 0; x < sizeX; x++) {
			for (int y = 0; y < sizeY; y++) {
				for (int z = 0; z < sizeZ; z++) {
					fmat44 modelMatrix = TransformationMatrices::getTranslationMatrix(
						y - (float)sizeY/2 - 10,
						z - (float)sizeZ/2,
						x - (float)sizeX/2
					);
					fmat44 MVP = viewProjectionMatrix * modelMatrix;
					fmat44 MV = viewMatrix * modelMatrix;
					_cubes[SUBSPACE_XYW][sizeY*sizeZ*x + sizeZ*y + z] = GridCube(_gridShaderProgram, viewMatrix, MV, MVP, grid(x,y,z));
				}
			}
		}
		sort(_cubes[SUBSPACE_XYW].begin(), _cubes[SUBSPACE_XYW].end());
	});
	
	future<void> futXzw = async([&]() {
		const Cube<gridCell>& grid = gameFieldSubspaces[SUBSPACE_XZW].getGrid();
		unsigned int sizeX = gameFieldSubspaces[SUBSPACE_XZW].getSizeX();
		unsigned int sizeY = gameFieldSubspaces[SUBSPACE_XZW].getSizeY();
		unsigned int sizeZ = gameFieldSubspaces[SUBSPACE_XZW].getSizeZ();
		if (_cubes[SUBSPACE_XZW].size() == 0) {
			_cubes[SUBSPACE_XZW].reserve(grid.n_elem);
			_cubes[SUBSPACE_XZW].resize(grid.n_elem);
		}
		for (int x = 0; x < sizeX; x++) {
			for (int y = 0; y < sizeY; y++) {
				for (int z = 0; z < sizeZ; z++) {
					fmat44 modelMatrix = TransformationMatrices::getTranslationMatrix(
						x - (float)sizeX/2 + 5,
						y - (float)sizeY/2,
						z - (float)sizeZ/2
					);
					fmat44 MVP = viewProjectionMatrix * modelMatrix;
					fmat44 MV = viewMatrix * modelMatrix;
					_cubes[SUBSPACE_XZW][sizeY*sizeZ*x + sizeZ*y + z] = GridCube(_gridShaderProgram, viewMatrix, MV, MVP, grid(x,y,z));
				}
			}
		}
		sort(_cubes[SUBSPACE_XZW].begin(), _cubes[SUBSPACE_XZW].end());
	});
	
	future<void> futYzw = async([&]() {
		const Cube<gridCell>& grid = gameFieldSubspaces[SUBSPACE_YZW].getGrid();
		unsigned int sizeX = gameFieldSubspaces[SUBSPACE_YZW].getSizeX();
		unsigned int sizeY = gameFieldSubspaces[SUBSPACE_YZW].getSizeY();
		unsigned int sizeZ = gameFieldSubspaces[SUBSPACE_YZW].getSizeZ();
		if (_cubes[SUBSPACE_YZW].size() == 0) {
			_cubes[SUBSPACE_YZW].reserve(grid.n_elem);
			_cubes[SUBSPACE_YZW].resize(grid.n_elem);
		}
		for (int x = 0; x < sizeX; x++) {
			for (int y = 0; y < sizeY; y++) {
				for (int z = 0; z < sizeZ; z++) {
					fmat44 modelMatrix = TransformationMatrices::getTranslationMatrix(
						z - (float)sizeZ/2 + 20,
						y - (float)sizeY/2,
						x - (float)sizeX/2
					);
					fmat44 MVP = viewProjectionMatrix * modelMatrix;
					fmat44 MV = viewMatrix * modelMatrix;
					_cubes[SUBSPACE_YZW][sizeY*sizeZ*x + sizeZ*y + z] = GridCube(_gridShaderProgram, viewMatrix, MV, MVP, grid(x,y,z));
				}
			}
		}
		sort(_cubes[SUBSPACE_YZW].begin(), _cubes[SUBSPACE_YZW].end());
	});
	futXyz.wait();
	futXyw.wait();
	futXzw.wait();
	futYzw.wait();

	cubes.reserve(_cubes[SUBSPACE_XYZ].size() + _cubes[SUBSPACE_XYW].size() + _cubes[SUBSPACE_XZW].size() + _cubes[SUBSPACE_YZW].size());
	if (_cubes[SUBSPACE_XYZ][0] < _cubes[SUBSPACE_YZW][0]) {
		cubes.insert(cubes.end(), _cubes[SUBSPACE_XYZ].begin(), _cubes[SUBSPACE_XYZ].end());
		cubes.insert(cubes.end(), _cubes[SUBSPACE_XYW].begin(), _cubes[SUBSPACE_XYW].end());
		cubes.insert(cubes.end(), _cubes[SUBSPACE_XZW].begin(), _cubes[SUBSPACE_XZW].end());
		cubes.insert(cubes.end(), _cubes[SUBSPACE_YZW].begin(), _cubes[SUBSPACE_YZW].end());
	} else {
		cubes.insert(cubes.end(), _cubes[SUBSPACE_YZW].begin(), _cubes[SUBSPACE_YZW].end());
		cubes.insert(cubes.end(), _cubes[SUBSPACE_XZW].begin(), _cubes[SUBSPACE_XZW].end());
		cubes.insert(cubes.end(), _cubes[SUBSPACE_XYW].begin(), _cubes[SUBSPACE_XYW].end());
		cubes.insert(cubes.end(), _cubes[SUBSPACE_XYZ].begin(), _cubes[SUBSPACE_XYZ].end());
	}

	for (auto &cube : cubes) {
		cube.draw();
	}

	glDisableVertexAttribArray(_gridShaderProgram->getAttribLocation("vPosition"));
	glDisableVertexAttribArray(_gridShaderProgram->getAttribLocation("vLinesPosition"));
	//glDeleteBuffers(1, &vertexbuffer);
	//glDeleteBuffers(1, &indexbuffer);
	glDeleteBuffers(1, &trianglesbuffer);
	glDeleteBuffers(1, &linebuffer);
	//glDeleteBuffers(1, &sidesbuffer);

	/*//prepare data for grid shader program
	GLfloat testDots[] = {
		0,0,0,
		1,1,1,
		2,2,2
	};
	GLfloat testTypes[] = {0,1,2};

	GLuint testDotsBuffer = 0;
    glGenBuffers(1, &testDotsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, testDotsBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(testDots), testDots, GL_STATIC_DRAW);
	GLuint testTypesBuffer = 0;
	glGenBuffers(1, &testTypesBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, testTypesBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(testTypes), testTypes, GL_STATIC_DRAW);

	glEnableVertexAttribArray(_gridShaderProgram->getAttribLocation("vCubePosition"));
    glBindBuffer(GL_ARRAY_BUFFER, testDotsBuffer);
    glVertexAttribPointer(
            _gridShaderProgram->getAttribLocation("vCubePosition"),
            3,			//size
            GL_FLOAT,	//type
            GL_FALSE,	//normalized?
            0,			//stride
            (void*)0	//array buffer offset
    );
	glEnableVertexAttribArray(_gridShaderProgram->getAttribLocation("vCubeType"));
	glBindBuffer(GL_ARRAY_BUFFER, testTypesBuffer);
	glVertexAttribPointer(
		_gridShaderProgram->getAttribLocation("vCubeType"),
		1,				//size
		GL_FLOAT,			//type
		GL_FALSE,		//normalized?
		0,				//stride
		(void*)0		//array buffer offset
	);
	
	glUniformMatrix4fv(_gridShaderProgram->getUniformLocation("M"), 1, GL_FALSE, (fmat44(4,4).eye()).mem); //TODO: MVP matrix
	glUniformMatrix4fv(_gridShaderProgram->getUniformLocation("V"), 1, GL_FALSE, getViewMatrix().mem); //TODO: MVP matrix
	glUniformMatrix4fv(_gridShaderProgram->getUniformLocation("P"), 1, GL_FALSE, getProjectionMatrix().mem); //TODO: MVP matrix
	glUniformMatrix4fv(_gridShaderProgram->getUniformLocation("MVP"), 1, GL_FALSE, _viewProjectionMatrix.mem); //TODO: MVP matrix

	glDrawArrays(GL_POINTS, 0, 3);
	glDisableVertexAttribArray(_gridShaderProgram->getAttribLocation("vCubePosition"));
	glDisableVertexAttribArray(_gridShaderProgram->getAttribLocation("vCubeType"));
	glDeleteBuffers(1, &testDotsBuffer);
	glDeleteBuffers(1, &testTypesBuffer);*/
	/*//SUBSPACE_XYZ
	GLfloat xAxisCoords[] = {
		-(float)gameFieldSubspaces[SUBSPACE_XYZ].getSizeY()/2, -(float)gameFieldSubspaces[SUBSPACE_XYZ].getSizeZ()/2, -(float)gameFieldSubspaces[SUBSPACE_XYZ].getSizeX()/2, //x axis
		-(float)gameFieldSubspaces[SUBSPACE_XYZ].getSizeY()/2, -(float)gameFieldSubspaces[SUBSPACE_XYZ].getSizeZ()/2, (float)gameFieldSubspaces[SUBSPACE_XYZ].getSizeX()/2,

		-(float)gameFieldSubspaces[SUBSPACE_XYZ].getSizeY()/2, -(float)gameFieldSubspaces[SUBSPACE_XYZ].getSizeZ()/2, -(float)gameFieldSubspaces[SUBSPACE_XYZ].getSizeX()/2, //y axis
		(float)gameFieldSubspaces[SUBSPACE_XYZ].getSizeY()/2,  -(float)gameFieldSubspaces[SUBSPACE_XYZ].getSizeZ()/2, -(float)gameFieldSubspaces[SUBSPACE_XYZ].getSizeX()/2,

		-(float)gameFieldSubspaces[SUBSPACE_XYZ].getSizeY()/2, -(float)gameFieldSubspaces[SUBSPACE_XYZ].getSizeZ()/2, -(float)gameFieldSubspaces[SUBSPACE_XYZ].getSizeX()/2, //z axis
		-(float)gameFieldSubspaces[SUBSPACE_XYZ].getSizeY()/2, (float)gameFieldSubspaces[SUBSPACE_XYZ].getSizeZ()/2,  -(float)gameFieldSubspaces[SUBSPACE_XYZ].getSizeX()/2,
	};
	GLfloat xAxisColors[] = {
		1.0f,0.0f,0.0f, //x axis - red
		1.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f, //y axis - green
		0.0f,1.0f,0.0f,
		0.0f,0.0f,1.0f, //z axis - blue
		0.0f,0.0f,1.0f
	};
	GLuint xAxisCoordsBuffer = 0;
    glGenBuffers(1, &xAxisCoordsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, xAxisCoordsBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(xAxisCoords), xAxisCoords, GL_STATIC_DRAW);
	GLuint xAxisColorsBuffer = 0;
	glGenBuffers(1, &xAxisColorsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, xAxisColorsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(xAxisColors), xAxisColors, GL_STATIC_DRAW);

	glEnableVertexAttribArray(_shaderProgram.getAttribLocation("vertexPosition"));
    glBindBuffer(GL_ARRAY_BUFFER, xAxisCoordsBuffer);
    glVertexAttribPointer(
            _shaderProgram.getAttribLocation("vertexPosition"),
            3,			//size
            GL_FLOAT,	//type
            GL_FALSE,	//normalized?
            0,			//stride
            (void*)0	//array buffer offset
    );
	glEnableVertexAttribArray(_shaderProgram.getAttribLocation("vertexColor"));
	glBindBuffer(GL_ARRAY_BUFFER, xAxisColorsBuffer);
	glVertexAttribPointer(
		_shaderProgram.getAttribLocation("vertexColor"),
		3,				//size
		GL_FLOAT,		//type
		GL_FALSE,		//normalized?
		0,				//stride
		(void*)0		//array buffer offset
	);

	glUniform1f(_shaderProgram.getUniformLocation("vertexAlpha"), 1.0f);

	glUniformMatrix4fv(_shaderProgram.getUniformLocation("modelViewProjectionMatrix"), 1, GL_FALSE, _viewProjectionMatrix.mem); //TODO: MVP matrix

	glDrawArrays(GL_LINES, 0, 6); //2 coordinates
	glDisableVertexAttribArray(_shaderProgram.getAttribLocation("vertexPosition"));
	glDisableVertexAttribArray(_shaderProgram.getAttribLocation("vertexColor"));
	glDeleteBuffers(1, &xAxisCoordsBuffer);
	glDeleteBuffers(1, &xAxisColorsBuffer);*/



	
	/*unsigned int numberofGridLines = (gameFieldSubspaces[SUBSPACE_XYZ].getSizeX() + 1) * (gameFieldSubspaces[SUBSPACE_XYZ].getSizeY() + 1) * (gameFieldSubspaces[SUBSPACE_XYZ].getSizeZ() + 1);
	GLfloat* gridLinesCoords = new GLfloat[numberofGridLines*3*2];
	GLfloat* gridLinesColors = new GLfloat[numberofGridLines*3*2];
	int l = 0;
	int m = 0;
	for (int i = 0; i < (gameFieldSubspaces[SUBSPACE_XYZ].getSizeX() + 1); i++) {
		for (int j = 0; j < (gameFieldSubspaces[SUBSPACE_XYZ].getSizeY() + 1); j++) {
			for (int k = 0; k < (gameFieldSubspaces[SUBSPACE_XYZ].getSizeZ() + 1); k++) {
				GLfloat coords[] = {
					-(float)gameFieldSubspaces[SUBSPACE_XYZ].getSizeY()/2, -(float)gameFieldSubspaces[SUBSPACE_XYZ].getSizeZ()/2, -(float)gameFieldSubspaces[SUBSPACE_XYZ].getSizeX()/2, //x axis
					-(float)gameFieldSubspaces[SUBSPACE_XYZ].getSizeY()/2+j, -(float)gameFieldSubspaces[SUBSPACE_XYZ].getSizeZ()/2+k, -(float)gameFieldSubspaces[SUBSPACE_XYZ].getSizeX()/2+i,
				};
				gridLinesCoords[l++] = coords[0]; //start
				gridLinesCoords[l++] = coords[1];
				gridLinesCoords[l++] = coords[2];
				gridLinesCoords[l++] = coords[3]; //end
				gridLinesCoords[l++] = coords[4];
				gridLinesCoords[l++] = coords[5];
				gridLinesColors[m++] = 1.0f;
				gridLinesColors[m++] = 1.0f;
				gridLinesColors[m++] = 1.0f;
				gridLinesColors[m++] = 1.0f;
				gridLinesColors[m++] = 1.0f;
				gridLinesColors[m++] = 1.0f;
			}
		}
	}
	GLuint gridLinesCoordsBuffer = 0;
    glGenBuffers(1, &gridLinesCoordsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, gridLinesCoordsBuffer);
    glBufferData(GL_ARRAY_BUFFER, numberofGridLines*3*2*sizeof(GLfloat), gridLinesCoords, GL_STATIC_DRAW);
	GLuint gridLinesColorsBuffer = 0;
	glGenBuffers(1, &gridLinesColorsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, gridLinesColorsBuffer);
	glBufferData(GL_ARRAY_BUFFER, numberofGridLines*3*2*sizeof(GLfloat), gridLinesColors, GL_STATIC_DRAW);

	glEnableVertexAttribArray(_shaderProgram.getAttribLocation("vertexPosition"));
    glBindBuffer(GL_ARRAY_BUFFER, gridLinesCoordsBuffer);
    glVertexAttribPointer(
            _shaderProgram.getAttribLocation("vertexPosition"),
            3,			//size
            GL_FLOAT,	//type
            GL_FALSE,	//normalized?
            0,			//stride
            (void*)0	//array buffer offset
    );
	glEnableVertexAttribArray(_shaderProgram.getAttribLocation("vertexColor"));
	glBindBuffer(GL_ARRAY_BUFFER, gridLinesColorsBuffer);
	glVertexAttribPointer(
		_shaderProgram.getAttribLocation("vertexColor"),
		3,				//size
		GL_FLOAT,		//type
		GL_FALSE,		//normalized?
		0,				//stride
		(void*)0		//array buffer offset
	);

	glUniform1f(_shaderProgram.getUniformLocation("vertexAlpha"), 0.5f);

	glUniformMatrix4fv(_shaderProgram.getUniformLocation("modelViewProjectionMatrix"), 1, GL_FALSE, _viewProjectionMatrix.mem); //TODO: MVP matrix

	glDrawArrays(GL_LINES, 0, numberofGridLines*2);
	glDisableVertexAttribArray(_shaderProgram.getAttribLocation("vertexPosition"));
	glDisableVertexAttribArray(_shaderProgram.getAttribLocation("vertexColor"));
	glDeleteBuffers(1, &gridLinesCoordsBuffer);
	glDeleteBuffers(1, &gridLinesColorsBuffer);
	delete[] gridLinesCoords;
	delete[] gridLinesColors;*/

	

	/*//<line>
	GLfloat lineCoordsBuffer[] = {
		0.0f, 0.5f,5.0f,
		-0.5f,-0.5f,5.0f
	};
	GLfloat lineColorsBuffer[] = {
		1.0f,0.0f,0.0f, //start
		1.0f,0.0f,0.0f //end
	};

	GLuint lineCoordBuffer = 0;
    glGenBuffers(1, &lineCoordBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, lineCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineCoordsBuffer), lineCoordsBuffer, GL_STATIC_DRAW);

	GLuint lineColorBuffer = 0;
	glGenBuffers(1, &lineColorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, lineColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineColorsBuffer), lineColorsBuffer, GL_STATIC_DRAW);

	glEnableVertexAttribArray(_shaderProgram.getAttribLocation("vertexPosition"));
    glBindBuffer(GL_ARRAY_BUFFER, lineCoordBuffer);
    glVertexAttribPointer(
            _shaderProgram.getAttribLocation("vertexPosition"),
            3,			//size
            GL_FLOAT,	//type
            GL_FALSE,	//normalized?
            0,			//stride
            (void*)0	//array buffer offset
    );

	glEnableVertexAttribArray(_shaderProgram.getAttribLocation("vertexColor"));
	glBindBuffer(GL_ARRAY_BUFFER, lineColorBuffer);
	glVertexAttribPointer(
		_shaderProgram.getAttribLocation("vertexColor"),
		3,				//size
		GL_FLOAT,		//type
		GL_FALSE,		//normalized?
		0,				//stride
		(void*)0		//array buffer offset
	);

	glUniform1f(_shaderProgram.getUniformLocation("vertexAlpha"), 0.3f);

	glUniformMatrix4fv(_shaderProgram.getUniformLocation("modelViewProjectionMatrix"), 1, GL_FALSE, ((fmat44)(getProjectionMatrix()*getTranslationMatrix(0,-0.5,-7)*getRotationXZMatrix(25))).mem);

	//Draw line
	glDrawArrays(GL_LINES, 0, 2); //2 coordinates

	glDisableVertexAttribArray(_shaderProgram.getAttribLocation("vertexPosition"));
	glDisableVertexAttribArray(_shaderProgram.getAttribLocation("vertexColor"));

	glDeleteBuffers(1, &lineCoordBuffer);
	glDeleteBuffers(1, &lineColorBuffer);
	//</line>

	//<cube>
	//Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
	//A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices.
	GLfloat cubeMeshesBuffer[] = {
		-1.0f,-1.0f,-1.0f, // triangle 1 : begin
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, // triangle 1 : end
		1.0f, 1.0f,-1.0f, // triangle 2 : begin
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f, // triangle 2 : end
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f
	};

	// One color for each vertex. They were generated randomly.
	GLfloat cubeColorsBuffer[] = {
		0.583f, 0.771f, 0.014f,
		0.609f, 0.115f, 0.436f,
		0.327f, 0.483f, 0.844f,
		0.822f, 0.569f, 0.201f,
		0.435f, 0.602f, 0.223f,
		0.310f, 0.747f, 0.185f,
		0.597f, 0.770f, 0.761f,
		0.559f, 0.436f, 0.730f,
		0.359f, 0.583f, 0.152f,
		0.483f, 0.596f, 0.789f,
		0.559f, 0.861f, 0.639f,
		0.195f, 0.548f, 0.859f,
		0.014f, 0.184f, 0.576f,
		0.771f, 0.328f, 0.970f,
		0.406f, 0.615f, 0.116f,
		0.676f, 0.977f, 0.133f,
		0.971f, 0.572f, 0.833f,
		0.140f, 0.616f, 0.489f,
		0.997f, 0.513f, 0.064f,
		0.945f, 0.719f, 0.592f,
		0.543f, 0.021f, 0.978f,
		0.279f, 0.317f, 0.505f,
		0.167f, 0.620f, 0.077f,
		0.347f, 0.857f, 0.137f,
		0.055f, 0.953f, 0.042f,
		0.714f, 0.505f, 0.345f,
		0.783f, 0.290f, 0.734f,
		0.722f, 0.645f, 0.174f,
		0.302f, 0.455f, 0.848f,
		0.225f, 0.587f, 0.040f,
		0.517f, 0.713f, 0.338f,
		0.053f, 0.959f, 0.120f,
		0.393f, 0.621f, 0.362f,
		0.673f, 0.211f, 0.457f,
		0.820f, 0.883f, 0.371f,
		0.982f, 0.099f, 0.879f
	};

	GLuint cubeVertexBuffer = 0;
    glGenBuffers(1, &cubeVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeMeshesBuffer), cubeMeshesBuffer, GL_STATIC_DRAW);

	GLuint cubeColorBuffer = 0;
	glGenBuffers(1, &cubeColorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, cubeColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeColorsBuffer), cubeColorsBuffer, GL_STATIC_DRAW);

	//vertices
	glEnableVertexAttribArray(_shaderProgram.getAttribLocation("vertexPosition"));
    glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuffer);
    glVertexAttribPointer(
            _shaderProgram.getAttribLocation("vertexPosition"),
            3,			//size
            GL_FLOAT,	//type
            GL_FALSE,	//normalized?
            0,			//stride
            (void*)0	//array buffer offset
    );

	//colors
	glEnableVertexAttribArray(_shaderProgram.getAttribLocation("vertexColor"));
	glBindBuffer(GL_ARRAY_BUFFER, cubeColorBuffer);
	glVertexAttribPointer(
		_shaderProgram.getAttribLocation("vertexColor"),
		3,				//size
		GL_FLOAT,		//type
		GL_FALSE,		//normalized?
		0,				//stride
		(void*)0		//array buffer offset
	);

	//set alpha
	glUniform1f(_shaderProgram.getUniformLocation("vertexAlpha"), 0.75f);

	//projection matrix
	//TODO: MVP = Projection * View * Model
	static int rot = 0;
	rot++;
	glUniformMatrix4fv(_shaderProgram.getUniformLocation("modelViewProjectionMatrix"), 1, GL_FALSE, ((fmat44)(getProjectionMatrix()*getTranslationMatrix(4,-2,-7)*getRotationXZMatrix(rot))).mem);
	
	//Draw cube triangles
	glDrawArrays(GL_TRIANGLES, 0, 12*3); //12*3 indices starting at 0 -> 12 triangles -> 6 squares

	glDisableVertexAttribArray(_shaderProgram.getAttribLocation("vertexPosition"));
	glDisableVertexAttribArray(_shaderProgram.getAttribLocation("vertexColor"));

	glDeleteBuffers(1, &cubeVertexBuffer);
	glDeleteBuffers(1, &cubeColorBuffer);
	//</cube>

	//<triangle>
	GLfloat triangleMeshesBuffer[] = {
		 0.0f, 0.5f,5.0f,
		-0.5f,-0.5f,5.0f,
		 0.5f,-0.5f,5.0f
	};
	GLfloat triangleColorsBuffer[] = {
		0.5f,0.0f,0.0f,
		0.0f,0.5f,0.0f,
		0.0f,0.0f,0.5f
	};

	GLuint triangleVertexBuffer = 0;
	glGenBuffers(1, &triangleVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, triangleVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleMeshesBuffer), triangleMeshesBuffer, GL_STATIC_DRAW);

	GLuint triangleColorBuffer = 0;
	glGenBuffers(1, &triangleColorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, triangleColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleColorsBuffer), triangleColorsBuffer, GL_STATIC_DRAW);

	glEnableVertexAttribArray(_shaderProgram.getAttribLocation("vertexPosition"));
    glBindBuffer(GL_ARRAY_BUFFER, triangleVertexBuffer);
    glVertexAttribPointer(
            _shaderProgram.getAttribLocation("vertexPosition"),
            3,			//size
            GL_FLOAT,	//type
            GL_FALSE,	//normalized?
            0,			//stride
            (void*)0	//array buffer offset
    );

	glEnableVertexAttribArray(_shaderProgram.getAttribLocation("vertexColor"));
	glBindBuffer(GL_ARRAY_BUFFER, triangleColorBuffer);
	glVertexAttribPointer(
		_shaderProgram.getAttribLocation("vertexColor"),
		3,				//size
		GL_FLOAT,		//type
		GL_FALSE,		//normalized?
		0,				//stride
		(void*)0		//array buffer offset
	);

	//set alpha
	glUniform1f(_shaderProgram.getUniformLocation("vertexAlpha"), 0.5f);

	glUniformMatrix4fv(_shaderProgram.getUniformLocation("modelViewProjectionMatrix"), 1, GL_FALSE, ((fmat44)(getProjectionMatrix()*getTranslationMatrix(0,0,-7)*getRotationXZMatrix(10))).mem);

	glDrawArrays(GL_TRIANGLES, 0, 3); //3 indices starting at 0 -> 1 triangle

	glDisableVertexAttribArray(_shaderProgram.getAttribLocation("vertexPosition"));
	glDisableVertexAttribArray(_shaderProgram.getAttribLocation("vertexColor"));

	glDeleteBuffers(1, &triangleVertexBuffer);
	glDeleteBuffers(1, &triangleColorBuffer);
	//</triangle>*/

	//check for errors
	//OPENGL_CHECK_FOR_ERRORS();
}



/*
	  E----F
	 /|   /|
	A----B |
	|-H--|-G
	|    |/
	D----C
*/
const GLfloat Render::GridCube::_cubeVertices[] = {
	-0.5, +0.5, +0.5, //A - 0
	+0.5, +0.5, +0.5, //B - 1
	+0.5, -0.5, +0.5, //C - 2
	-0.5, -0.5, +0.5, //D - 3
	-0.5, +0.5, -0.5, //E - 4
	+0.5, +0.5, -0.5, //F - 5
	+0.5, -0.5, -0.5, //G - 6
	-0.5, -0.5, -0.5, //H - 7
};
const GLuint Render::GridCube::_cubeIndices[] = {
	0,2,1, 0,3,2, //front
	7,0,4, 7,3,0, //left
	1,6,5, 1,2,6, //right
	3,6,2, 3,7,6, //bottom
	4,1,5, 4,0,1, //top
	4,5,6, 4,6,7  //back
};
const GLfloat Render::GridCube::_cubeTriangles[] = {
	//front:
	-0.5, +0.5, +0.5, //A - 0
	+0.5, -0.5, +0.5, //C - 2
	+0.5, +0.5, +0.5, //B - 1
	-0.5, +0.5, +0.5, //A - 0
	-0.5, -0.5, +0.5, //D - 3
	+0.5, -0.5, +0.5, //C - 2

	//left:
	-0.5, -0.5, -0.5, //H - 7
	-0.5, +0.5, +0.5, //A - 0
	-0.5, +0.5, -0.5, //E - 4
	-0.5, -0.5, -0.5, //H - 7
	-0.5, -0.5, +0.5, //D - 3
	-0.5, +0.5, +0.5, //A - 0

	//right:
	+0.5, +0.5, +0.5, //B - 1
	+0.5, -0.5, -0.5, //G - 6
	+0.5, +0.5, -0.5, //F - 5
	+0.5, +0.5, +0.5, //B - 1
	+0.5, -0.5, +0.5, //C - 2
	+0.5, -0.5, -0.5, //G - 6

	//bottom:
	-0.5, -0.5, +0.5, //D - 3
	+0.5, -0.5, -0.5, //G - 6
	+0.5, -0.5, +0.5, //C - 2
	-0.5, -0.5, +0.5, //D - 3
	-0.5, -0.5, -0.5, //H - 7
	+0.5, -0.5, -0.5, //G - 6

	//top:
	-0.5, +0.5, -0.5, //E - 4
	+0.5, +0.5, +0.5, //B - 1
	+0.5, +0.5, -0.5, //F - 5
	-0.5, +0.5, -0.5, //E - 4
	-0.5, +0.5, +0.5, //A - 0
	+0.5, +0.5, +0.5, //B - 1

	//back:
	-0.5, +0.5, -0.5, //E - 4
	+0.5, +0.5, -0.5, //F - 5
	+0.5, -0.5, -0.5, //G - 6
	-0.5, +0.5, -0.5, //E - 4
	+0.5, -0.5, -0.5, //G - 6
	-0.5, -0.5, -0.5, //H - 7
};
const GLubyte Render::GridCube::_cubeSides[] = {
	'f','f','f', 'f','f','f', //front
	'l','l','l', 'l','l','l', //left
	'r','r','r', 'r','r','r', //right
	'o','o','o', 'o','o','o', //bottom
	't','t','t', 't','t','t', //top
	'a','a','a', 'a','a','a'  //back
};
/*
	  E----F
	 /|   /|
	A----B |
	|-H--|-G
	|    |/
	D----C
*/
const GLfloat Render::GridCube::_cubeLines[] = {
	//front:
	-0.5, +0.5, +0.5, //A - 0
	+0.5, +0.5, +0.5, //B - 1
	+0.5, +0.5, +0.5, //B - 1
	+0.5, -0.5, +0.5, //C - 2
	+0.5, -0.5, +0.5, //C - 2
	-0.5, -0.5, +0.5, //D - 3	
	-0.5, -0.5, +0.5, //D - 3
	-0.5, +0.5, +0.5, //A - 0

	//left:
	-0.5, +0.5, +0.5, //A - 0
	-0.5, +0.5, -0.5, //E - 4
	-0.5, +0.5, -0.5, //E - 4
	-0.5, -0.5, -0.5, //H - 7
	-0.5, -0.5, -0.5, //H - 7
	-0.5, -0.5, +0.5, //D - 3

	//right:
	+0.5, +0.5, +0.5, //B - 1
	+0.5, +0.5, -0.5, //F - 5
	+0.5, +0.5, -0.5, //F - 5
	+0.5, -0.5, -0.5, //G - 6
	+0.5, -0.5, -0.5, //G - 6
	+0.5, -0.5, +0.5, //C - 2

	//bottom:
	+0.5, -0.5, -0.5, //G - 6
	-0.5, -0.5, -0.5, //H - 7

	//top:
	-0.5, +0.5, -0.5, //E - 4
	+0.5, +0.5, -0.5, //F - 5
};

void Render::GridCube::draw() {
	glUniformMatrix4fv(_shaderProgram->getUniformLocation("V"), 1, GL_FALSE, _V.mem);
	glUniformMatrix4fv(_shaderProgram->getUniformLocation("MV"), 1, GL_FALSE, _MV.mem);
	glUniformMatrix4fv(_shaderProgram->getUniformLocation("MVP"), 1, GL_FALSE, _MVP.mem);
	glUniform4f(_shaderProgram->getUniformLocation("cubeCenter"), _MVP(0,3), _MVP(1,3), _MVP(2,3), _MVP(3,3));
	if (_type == GRIDCELL_NOTHING) {
		glUniform4f(_shaderProgram->getUniformLocation("shapeColor"), 0.2f, 0.2f, 0.2f, 0.5f);
		glUniform1f(_shaderProgram->getUniformLocation("isLine"), 1.0f);
		//glDrawElements(GL_TRIANGLES, 12*3, GL_UNSIGNED_INT, NULL);
		glDrawArrays(GL_LINES, 0, 12*2);
	} else if (_type == GRIDCELL_PIECE_BRICK) {
		glUniform4f(_shaderProgram->getUniformLocation("shapeColor"), 0.9f, 0.0f, 0.0f, 0.9f);
		glUniform1f(_shaderProgram->getUniformLocation("isLine"), 0.0f);
		//glDrawElements(GL_TRIANGLES, 12*3, GL_UNSIGNED_INT, NULL);
		glDrawArrays(GL_TRIANGLES, 0, 12*3);
	} else if (_type == GRIDCELL_CUP_BRICK) {
		glUniform4f(_shaderProgram->getUniformLocation("shapeColor"), 0.0f, 0.0f, 0.9f, 0.7f);
		glUniform1f(_shaderProgram->getUniformLocation("isLine"), 0.0f);
		//glDrawElements(GL_TRIANGLES, 12*3, GL_UNSIGNED_INT, NULL);
		glDrawArrays(GL_TRIANGLES, 0, 12*3);
	}
}

bool Render::GridCube::operator<(const GridCube &other) const {
	return (_MVP(2,3)/_MVP(3,3) > other._MVP(2,3)/other._MVP(3,3));
}


Render& operator<< (Render &g, const Zoom &m) {
	g._camera.lock();
	/*//TODO: zoom to
	POINT mouseCoordinates = m.mouseScreenCoordinates;
	ScreenToClient(g._windowPtr->getHwnd(), &mouseCoordinates);
	POINT centerCoordinates = { g._windowPtr->getWidth() / 2, g._windowPtr->getHeight() / 2 };
	float basisdX = -(centerCoordinates.x - mouseCoordinates.x);
	float basisdY = centerCoordinates.y - mouseCoordinates.y;
	cout << basisdX << " " << basisdY << endl;
	//convert window coordinates to world coordinates
	fvec4 basis4;
	basis4.at(0) = basisdX;
	basis4.at(1) = basisdY;
	basis4.at(2) = 0;
	basis4.at(3) = 1;
	basis4 = g._camera.getViewProjectionMatrix() * basis4;
	cout << basis4 << endl;
	g._camera.circlingAroundMoveBasis(Camera::Coordinates(basis4.at(0)/basis4.at(3), basis4.at(1)/basis4.at(3), 0.0f));*/
	g._camera.zoom(m.zoom);
	g._camera.unlock();
	return g;
}

Render& operator<< (Render &g, const RenderMessages::Move &m) {
	g._camera.lock();
	g._camera.circlingAroundMoveBasis(Camera::Coordinates(m.x, m.y, 0.0f));
	g._camera.unlock();
	return g;
}
Render& operator<< (Render &g, const RenderMessages::Rotate &m) {
	g._camera.lock();
	g._camera.circlingAroundCircle(Camera::Angles(m.x, m.y, 0.0f));
	g._camera.unlock();
	return g;
}

Render& operator<< (Render &g, const RenderMessages::ResetView &m) {
	g._camera.lock();
	g._camera.resetAll();
	g._camera.unlock();
	return g;
}