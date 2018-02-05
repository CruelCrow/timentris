#include "Input.h"


Input::Input() : _qIn(new queue<MSG>(QUEUE_CAPACITY)), _running(false) {
}

Input::~Input(void) {
	stopInputLoop();
	delete _qIn;
}

void Input::attachGame(TimentrisGamePtr game) {
	if (isRunning()) {
		throw new InputExceptions::AlreadyRunning();
		return;
	}
	_gamePtr = game;
}

void Input::attachRender(RenderPtr render) {
	if (isRunning()) {
		throw new InputExceptions::AlreadyRunning();
		return;
	}
	_renderPtr = render;
}

void Input::run() {
	if (isRunning()) {
		throw new InputExceptions::AlreadyRunning();
		return;
	}
	if (_renderPtr == NULL) {
		throw new InputExceptions::NoRenderAttached();
		return;
	}
	if (_gamePtr == NULL) {
		throw new InputExceptions::NoGameAttached();
		return;
	}

	//Launch main input thread
	_inputLoopThreadPtr = new thread(inputLoop, this);
}

bool Input::isRunning() const {
	return (bool)_running;
}


void Input::inputLoop(Input* _this) {
	//wait until Render and Game are initialized
	while (
		_this->_renderPtr == NULL || !_this->_renderPtr->isRunning()
		||
		_this->_gamePtr == NULL || !_this->_gamePtr->isRunning()
		)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

	_this->_running = true;
	while (_this->_running) {
		MSG msg;
		if (_this->_qIn->pop(msg)) {
			//cout << msg.message << endl; //for test and debug only
			switch (msg.message) {
			case WM_MOUSEWHEEL:
				{
					POINT ptCurrentPoint = { 0 };
					GetCursorPos(&ptCurrentPoint);
					if (GET_WHEEL_DELTA_WPARAM(msg.wParam) > 0)
						*_this->_renderPtr << RenderMessages::Zoom(ptCurrentPoint, 1.05f);
					else
						*_this->_renderPtr << RenderMessages::Zoom(ptCurrentPoint, 0.95f);
					break;
				}
			case WM_MOUSEMOVE:
				{
					POINT ptCurrentPoint = { 0 };
					GetCursorPos(&ptCurrentPoint);
					MOUSEMOVEPOINT mmpSamplePoint = { 0 };
					mmpSamplePoint.x = ptCurrentPoint.x;
					mmpSamplePoint.y = ptCurrentPoint.y;
					MOUSEMOVEPOINT mmpRecordedMovements[2] = { 0 };
					GetMouseMovePointsEx(sizeof(mmpSamplePoint),
						&mmpSamplePoint,
						mmpRecordedMovements,
						2,
						GMMP_USE_DISPLAY_POINTS);
					int dX = mmpRecordedMovements[1].x - mmpSamplePoint.x;
					int dY = mmpRecordedMovements[1].y - mmpSamplePoint.y;
					switch (msg.wParam) {
					case MK_LBUTTON: //move
						{
							//TODO: freee mouse cursor
							*_this->_renderPtr << RenderMessages::Move((float)dX, (float)-dY);
							break;
						}
					case MK_RBUTTON: //rotate
						{
							//TODO: freee mouse cursor
							*_this->_renderPtr << RenderMessages::Rotate(-(float)dY, (float)dX);
							break;
						}
					}
					break;
				}
			case WM_MBUTTONUP:
				{
					*_this->_renderPtr << RenderMessages::ResetView();
					break;
				}
			case WM_KEYDOWN:
				{
					switch (msg.wParam) {
					case VK_NUMPAD5:
						*_this->_gamePtr << GameMessages::DescendFaster();
						break;
					case 'Q':
						*_this->_gamePtr << GameMessages::RotateXY();
						break;
					case 'W':
						*_this->_gamePtr << GameMessages::RotateXZ();
						break;
					case 'E':
						*_this->_gamePtr << GameMessages::RotateXW();
						break;
					case 'A':
						*_this->_gamePtr << GameMessages::RotateYZ();
						break;
					case 'S':
						*_this->_gamePtr << GameMessages::RotateYW();
						break;
					case 'Z':
						*_this->_gamePtr << GameMessages::RotateZW();
						break;
					case VK_NUMPAD4:
						*_this->_gamePtr << GameMessages::MoveYMinus1();
						break;
					case VK_NUMPAD6:
						*_this->_gamePtr << GameMessages::MoveY1();
						break;
					case VK_NUMPAD8:
						*_this->_gamePtr << GameMessages::MoveXMinus1();
						break;
					case VK_NUMPAD2:
						*_this->_gamePtr << GameMessages::MoveX1();
						break;
					case VK_NUMPAD7:
						*_this->_gamePtr << GameMessages::MoveWMinus1();
						break;
					case VK_NUMPAD9:
						*_this->_gamePtr << GameMessages::MoveW1();
						break;
					}
					break;
				}
			case WM_KEYUP:
				{
					//cout << msg.wParam << " " << msg.lParam << endl;
					switch (msg.wParam) {
					case VK_NUMPAD5:
						*_this->_gamePtr << GameMessages::CancelDescendFaster();
						break;
					}
					break;
				}
			}
		}
	}
}

void Input::stopInputLoop() {
	_running = false;
	if (_inputLoopThreadPtr) {
		_inputLoopThreadPtr->detach();
		delete _inputLoopThreadPtr;
		_inputLoopThreadPtr = NULL;
	}
}

Input& operator<< (Input &i, const MSG &m) {
	i._qIn->push(m);
	return i;
}