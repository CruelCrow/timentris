#pragma once
#include <string>
#include <iostream>
#include <atomic>
#include <boost/lexical_cast.hpp>
#include "exceptions.h"
#include "Configuration.h"
#include "OpenGL.h"
#include "Input.h"
#include "CircularReferenceFix.h"

using namespace std;

#define GLWINDOW_CLASS_NAME_BASE L"GLWindow_class"

class GLWindow {
	friend class Render;
	friend class Input;
private:
	static unsigned int _numberOfInstancesCreated;

	atomic<unsigned short> _width;
	atomic<unsigned short> _height;
	atomic<bool> _fullscreen;
	atomic<bool> _active;
	atomic<bool> _running;

	LARGE_INTEGER _qpc;
	double _timerFrequency;

	HINSTANCE _hInstance;
	HWND _hWnd;
	std::wstring _windowClassName;
	HDC _hDC;
	HGLRC _hRC;

public:
	GLWindow(int width, int height, bool isFullscreen);
	GLWindow(const GLWindow& other); //disallow copy constructor
	GLWindow& operator=(const GLWindow&); //disallow assignment operator
	~GLWindow(void);

	void setSize(int width, int height, bool fullScreen);

private:
	static LRESULT CALLBACK windowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

};
typedef shared_ptr<GLWindow> GLWindowPtr;
//TODO: disable screensaver