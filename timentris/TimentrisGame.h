#pragma once
#define __MSXML_LIBRARY_DEFINED__ //solve XMLDocument conflict error
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>
#include <boost/timer.hpp>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <memory>
#include "Brick.h"
#include "Piece.h"
#include "Cup.h"
#include "Configuration.h"
#include "Message.h"
#include "Render.h"
#include "exceptions.h"
#include "CircularReferenceFix.h"

namespace TimentrisMessages {
	namespace GameMessages {
		class GameMessage : public TimentrisMessages::Message {};
		
		class Stop : public GameMessage {};
		class Pause : public GameMessage {};
		class UnPause : public GameMessage {};

		class DescendFaster : public GameMessage {};
		class CancelDescendFaster : public GameMessage {};

		
		//Rotations:

		class Rotate : public GameMessage {}; //Virtual?
		class RotateXY : public Rotate {};
		class RotateXZ : public Rotate {};
		class RotateXW : public Rotate {};
		class RotateYZ : public Rotate {};
		class RotateYW : public Rotate {};
		class RotateZW : public Rotate {};


		//Moves:

		class Move : public GameMessage {}; //Virtual?

		class MoveX : public Move {
		public:
			int delta;
		};
		class MoveX1 : public MoveX {
		public:
			const int delta;
			MoveX1(void) : delta(1) {};
		};
		class MoveXMinus1 : public MoveX {
		public:
			const int delta;
			MoveXMinus1(void) : delta(-1) {};
		};


		class MoveY : public Move {
		public:
			int delta;
		};
		class MoveY1 : public MoveY {
		public:
			const int delta;
			MoveY1(void) : delta(1) {};
		};
		class MoveYMinus1 : public MoveY {
		public:
			const int delta;
			MoveYMinus1(void) : delta(-1) {};
		};


		class MoveW : public Move {
		public:
			int delta;
		};
		class MoveW1 : public MoveW {
		public:
			const int delta;
			MoveW1(void) : delta(1) {};
		};
		class MoveWMinus1 : public MoveW {
		public:
			const int delta;
			MoveWMinus1(void) : delta(-1) {};
		};
	}
}

#define GAME_FPS 120
#define GAME_FRAME_LENGTH_SEC 1/GAME_FPS // 1/120 sec
#define GAME_FRAME_LENGTH_MS GAME_FRAME_LENGTH_SEC*1000 //in milliseconds

#define GAME_FRAME_SCORE_MODIFIER 1
#define GAME_DEMOLISH_SCORE_MODIFIER 10000

#define GAME_FAST_DESCEND_FRAMES 6

using namespace std;
using namespace TimentrisMessages;
using namespace TimentrisMessages::GameMessages;

//Contains the main game logic
class TimentrisGame
{
private:
	Configuration::GamePreset _gamePresets;
	RenderPtr _renderPtr;
	Cup* _cupPtr;
	vector<Piece*> _availablePieces; //set of pieces avalable in the game (must be loaded on start)
	atomic<bool> _running; //TODO: stop running on window close
	atomic<bool> _paused; //TODO: pause on focus loose
	unsigned int _gameLevel; //game level
	bool _isLastGameLevel;
	unsigned long long _score; //player's score count
	bool _fastDescend; //drop Piece quickly (but not momenterary!) if this option is enabled

	void loadAvailablePieces();
	void unloadAvailablePieces();

	Piece nextPiece() const;

	void createCup();
	void destroyCup();

	thread _gameLoopThread;
	mutex _gameFrameMutex;
	static void gameLoop(TimentrisGame* _this);

	unsigned long long addGameScore(unsigned int score); //returns new score
public:
	unsigned long long getGameScore(); //return current game score
private:

	unsigned int nextGameLevel(); //go to next game level; returns new game level
public:
	unsigned int getGameLevel(); //return current game level
private:
	bool isGameover() const;

public:
	TimentrisGame(const string& gamePreset = "default");
	TimentrisGame(const TimentrisGame& other); //disallow copy constructor
	TimentrisGame& operator=(const TimentrisGame&); //disallow assignment operator
	~TimentrisGame(void);

	void lock(); //TODO
	void unlock();

	void attachRender(RenderPtr render);
	void run();
	bool isRunning() const;
	bool isPaused() const;

public:
	Cup* getCupPtr() const;

	//messages
	friend TimentrisGame& operator<< (TimentrisGame &g, const Stop &m); //TODO: stop when we close window
	friend TimentrisGame& operator<< (TimentrisGame &g, const Pause &m); //TODO: pause on focus loose
	friend TimentrisGame& operator<< (TimentrisGame &g, const UnPause &m); //TODO: unpause on focus return
	friend TimentrisGame& operator<< (TimentrisGame &g, const DescendFaster &m); //TODO: onKeyDown
	friend TimentrisGame& operator<< (TimentrisGame &g, const CancelDescendFaster &m); //TODO: onKeyUp
	friend TimentrisGame& operator<< (TimentrisGame &g, const RotateXY &m);
	friend TimentrisGame& operator<< (TimentrisGame &g, const RotateXZ &m);
	friend TimentrisGame& operator<< (TimentrisGame &g, const RotateXW &m);
	friend TimentrisGame& operator<< (TimentrisGame &g, const RotateYZ &m);
	friend TimentrisGame& operator<< (TimentrisGame &g, const RotateYW &m);
	friend TimentrisGame& operator<< (TimentrisGame &g, const RotateZW &m);
	friend TimentrisGame& operator<< (TimentrisGame &g, const MoveX1 &m);
	friend TimentrisGame& operator<< (TimentrisGame &g, const MoveXMinus1 &m);
	friend TimentrisGame& operator<< (TimentrisGame &g, const MoveY1 &m);
	friend TimentrisGame& operator<< (TimentrisGame &g, const MoveYMinus1 &m);
	friend TimentrisGame& operator<< (TimentrisGame &g, const MoveW1 &m);
	friend TimentrisGame& operator<< (TimentrisGame &g, const MoveWMinus1 &m);
};
typedef shared_ptr<TimentrisGame> TimentrisGamePtr;
//TODO: gameover
//TODO: unit tests
//TODO: send messages from game to render