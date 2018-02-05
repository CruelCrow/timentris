#pragma once
#include <thread>
#include <memory>
#include <atomic>
#include <chrono>
#include <boost/predef.h>
#include <boost/lockfree/queue.hpp>
#include "TimentrisGame.h"
#include "Render.h"
#include "CircularReferenceFix.h"

#if defined(BOOST_OS_WINDOWS)
#include <Windows.h>
#elif defined(BOOST_OS_LINUX)
//
#elif defined(BOOST_OS_MACOS)
//
#endif

using namespace std;
using namespace boost::lockfree;
using namespace TimentrisMessages;

#define QUEUE_CAPACITY 1024*10

class Input {
private:
	TimentrisGamePtr _gamePtr;
	RenderPtr _renderPtr;
	atomic<bool> _running;

	queue<MSG>* _qIn;

	thread* _inputLoopThreadPtr;
	static void inputLoop(Input* _this);
	void stopInputLoop();
public:
	Input();
	Input(const Input& other); //disallow copy constructor
	Input& operator=(const Input&); //disallow assignment operator
	~Input(void);

	void attachGame(TimentrisGamePtr game);
	void attachRender(RenderPtr render);
	void run();
	bool isRunning() const;

	friend Input& operator<< (Input &i, const MSG &m);
};
typedef shared_ptr<Input> InputPtr;

//TODO: split camera input and game input