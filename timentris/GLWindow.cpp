#include "GLWindow.h"

//init static memebrs
unsigned int GLWindow::_numberOfInstancesCreated = 0;

GLWindow::GLWindow(int width, int height, bool isFullscreen) {
	_numberOfInstancesCreated++;

	if (width < 0 || width > USHRT_MAX || height < 0 || height > USHRT_MAX) {
		cerr << __FILE__ << ":" << __LINE__ << " Wrong window screen width*height " << width << "*" << height << endl;
		throw new GLWindowExceptions::WrongScreenParameters();
		return;
	}
	_width = width;
	_height = height;
	_fullscreen = isFullscreen;
	_active = false;
	_running = false;

	WNDCLASSEX            wcx;
	PIXELFORMATDESCRIPTOR pfd;
	RECT                  rect;
	HGLRC                 hRCTemp;
	DWORD                 style, exStyle;
	int                   x, y, format;

	//Declare the pointer to OpenGL function for creating of the context
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;

	//OpenGL context attributes
	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_FLAGS_ARB,         WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	//Init performance timer
	QueryPerformanceFrequency(&_qpc);
	assert(_qpc.QuadPart > 0);

	_timerFrequency = 1.0 / _qpc.QuadPart;

	_hInstance = (HINSTANCE)GetModuleHandle(NULL);

	//Register window class
	memset(&wcx, 0, sizeof(wcx));
	wcx.cbSize        = sizeof(wcx);
	wcx.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcx.lpfnWndProc   = (WNDPROC)windowProc;
	wcx.hInstance     = _hInstance;
	_windowClassName = GLWINDOW_CLASS_NAME_BASE + boost::lexical_cast<std::wstring>(_numberOfInstancesCreated);
	wcx.lpszClassName = _windowClassName.c_str();
	wcx.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wcx.hCursor       = LoadCursor(NULL, IDC_ARROW);

	if (!RegisterClassEx(&wcx))	{
		cerr << "RegisterClassEx failed " << GetLastError() << endl;
		throw new GLWindowExceptions::FailedToCreateWindow();
		return;
	}

	//Window styles
	style   = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	exStyle = WS_EX_APPWINDOW;

	//place window at the cetner of the screen
	x = (GetSystemMetrics(SM_CXSCREEN) - width)  / 2;
	y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

	rect.left   = x;
	rect.right  = x + width;
	rect.top    = y;
	rect.bottom = y + height;

	//Adjust  window size to styles
	AdjustWindowRectEx(&rect, style, FALSE, exStyle);

	//Create a window
	_hWnd = CreateWindowEx(exStyle, wcx.lpszClassName, L"Timentris", style, rect.left, rect.top,
		rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, _hInstance, NULL);
	if (!_hWnd) {
		cerr << "CreateWindowEx failed " << GetLastError() << endl;
		throw new GLWindowExceptions::FailedToCreateWindow();
		return;
	}

	//Save the address of the cuurent class instance as the Window's USERDATA.
    SetWindowLong(_hWnd, GWLP_USERDATA, (long)this);

	//Get window context descriptor
	_hDC = GetDC(_hWnd);
	if (!_hDC)	{
		cerr << "GetDC failed " << GetLastError() << endl;
		throw new GLWindowExceptions::FailedToCreateWindow();
		return;
	}

	//Pixel format
	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize      = sizeof(pfd);
	pfd.nVersion   = 1;
	pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;

	//Set pixel format
	format = ChoosePixelFormat(_hDC, &pfd);
	if (!format || !SetPixelFormat(_hDC, format, &pfd))	{
		cerr << "Setting pixel format failed " << GetLastError() << endl;
		throw new GLWindowExceptions::FailedToCreateWindow();
		return;
	}

	//Create temporary redering context
	//We need to get wglCreateContextAttribsARB function
	hRCTemp = wglCreateContext(_hDC);
	if (!hRCTemp || !wglMakeCurrent(_hDC, hRCTemp))
	{
		cerr << "Ñreating temp render context failed " << GetLastError() << endl;
		throw new GLWindowExceptions::FailedToCreateWindow();
		return;
	}

	//Reterive address of the function for setting rendering context attributes
	OPENGL_GET_PROC(PFNWGLCREATECONTEXTATTRIBSARBPROC, wglCreateContextAttribsARB);

	//We do not need OpenGL temp context anymore
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRCTemp);

	//Create an extended context with support of OpenGL 3
	_hRC = wglCreateContextAttribsARB(_hDC, 0, attribs);
	if (!_hRC || !wglMakeCurrent(_hDC, _hRC)) {
		cerr << "Creating render context fail " << GetLastError() << endl;
		throw new OpenGLExceptions::FailedToCreateContext();
		return;
	}

	//Echo OpenGL debug info
	OpenGLPrintDebugInfo();

	//Load OpenGL extensions
	try {
		OpenGLInitExtensions();
	} catch (OpenGLExceptions::FailedToRetrieveFunctionAddress) {
		cerr << "Failed to load extensions " << GetLastError() << endl;
		throw new OpenGLExceptions::FailedToRetrieveFunctionAddress();
		return;
	}

	//Set window size
	setSize(_width, _height, _fullscreen);
}

GLWindow::~GLWindow(void) {
	_running = _active = false;

	//Restore screen resolution (if we where in fullscreen)
	if (_fullscreen) {
		ChangeDisplaySettings(NULL, CDS_RESET);
		ShowCursor(TRUE);
		_fullscreen = false;
	}

	//Remove rendering context
	if (_hRC) {
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(_hRC);
		_hRC = NULL;
	}

	//Release window context
	if (_hDC) {
		ReleaseDC(_hWnd, _hDC);
		_hDC = NULL;
	}

	//Remove window
	if (_hWnd) {
		DestroyWindow(_hWnd);
		_hWnd = NULL;
	}

	//Remove window class
	if (_hInstance)	{
		UnregisterClass(_windowClassName.c_str(), _hInstance);
		_hInstance = NULL;
	}
}

void GLWindow::setSize(int width, int height, bool fullScreen) {
	assert(width > 0);
	assert(height > 0);

	RECT    rect;
	DWORD   style, exStyle;
	DEVMODE devMode;
	LONG    result;
	int     x, y;

	//If we're returning from a fullscreen
	if (_fullscreen && !fullScreen) {
		ChangeDisplaySettings(NULL, CDS_RESET);
		ShowCursor(TRUE);
	}

	_fullscreen = fullScreen;

	//If fullscreen was required
	if (_fullscreen)
	{
		memset(&devMode, 0, sizeof(devMode));
		devMode.dmSize       = sizeof(devMode);
		devMode.dmPelsWidth  = width;
		devMode.dmPelsHeight = height;
		devMode.dmBitsPerPel = GetDeviceCaps(_hDC, BITSPIXEL);
		devMode.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

		//Try to set the fullscreen mode
		result = ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
		if (result != DISP_CHANGE_SUCCESSFUL)
		{
			cerr << "ChangeDisplaySettings failed " << width << "x" << height << " " << result << endl;
			_fullscreen = false;
		}
	}

	//If fullscreen was required and we successfully set it
	if (_fullscreen) {
		ShowCursor(TRUE);

		style   = WS_POPUP;
		exStyle = WS_EX_APPWINDOW | WS_EX_TOPMOST;

		x = y = 0;
	} else { //if it was not or we didn't succeed to set it
		style   = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		exStyle = WS_EX_APPWINDOW;

		//center window
		x = (GetSystemMetrics(SM_CXSCREEN) - width)  / 2;
		y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
	}

	rect.left   = x;
	rect.right  = x + width;
	rect.top    = y;
	rect.bottom = y + height;

	//Adjust  window size to styles
	AdjustWindowRectEx (&rect, style, FALSE, exStyle);

	//Set window styles
	SetWindowLong(_hWnd, GWL_STYLE,   style);
	SetWindowLong(_hWnd, GWL_EXSTYLE, exStyle);

	//Renew window position
	SetWindowPos(_hWnd, HWND_TOP, rect.left, rect.top,
		rect.right - rect.left, rect.bottom - rect.top,
		SWP_FRAMECHANGED);

	//Show window on screen
	ShowWindow(_hWnd, SW_SHOW);
	SetForegroundWindow(_hWnd);
	SetFocus(_hWnd);
	UpdateWindow(_hWnd);

	//Get window size
	GetClientRect(_hWnd, &rect);
	_width  = rect.right - rect.left;
	_height = rect.bottom - rect.top;

	//TOOD: Center the cursor

	//Running!
	_running = _active = true;

	OPENGL_CHECK_FOR_ERRORS();
}


LRESULT CALLBACK GLWindow::windowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	GLWindow *_this = (GLWindow*)GetWindowLong(hWnd, GWLP_USERDATA);
	if (!_this)
		return DefWindowProc(hWnd, msg, wParam, lParam);

	//cout << msg << " " << wParam << " " << lParam << endl; //debug

	switch (msg) {
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		{
			/*g_input.cursorPos[0] = (int)LOWORD(lParam);
			g_input.cursorPos[1] = (int)HIWORD(lParam);

			if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP)
				g_input.buttonState[0] = (msg == WM_LBUTTONDOWN ? INPUT_PRESSED : INPUT_UP);

			if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP)
				g_input.buttonState[1] = (msg == WM_RBUTTONDOWN ? INPUT_PRESSED : INPUT_UP);

			if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP)
				g_input.buttonState[2] = (msg == WM_MBUTTONDOWN ? INPUT_PRESSED : INPUT_UP);*/

			return FALSE;
		}

		case WM_MOUSEMOVE:
		{
			/*g_input.cursorPos[0] = (int)LOWORD(lParam);
			g_input.cursorPos[1] = (int)HIWORD(lParam);*/

			return FALSE;
		}

		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			/*if (wParam < 256 && (lParam & 0x40000000) == 0)
				g_input.keyState[wParam] = INPUT_PRESSED;*/

			return FALSE;
		}

		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			/*if (wParam < 256)
				g_input.keyState[wParam] = INPUT_UP;*/

			return FALSE;
		}

		case WM_SETFOCUS:
		case WM_KILLFOCUS:
		{
			_this->_active = (msg == WM_SETFOCUS);
			return FALSE;
		}

		case WM_ACTIVATE:
		{
			_this->_active = (LOWORD(wParam) != WA_INACTIVE);
			return FALSE;
		}

		case WM_CLOSE:
		{
			_this->_running = _this->_active = false;
			PostQuitMessage(0);
			return FALSE;
		}

		case WM_SYSCOMMAND:
		{
			switch (wParam & 0xFFF0)
			{
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
				{
					if (_this->_fullscreen)
						return FALSE;

					break;
				}

				case SC_KEYMENU:
				case SC_TASKLIST:
				{
					return FALSE;
				}
			}

			break;
		}

		case WM_ERASEBKGND:
		{
			return FALSE;
		}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}