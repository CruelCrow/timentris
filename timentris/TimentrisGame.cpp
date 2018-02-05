#include "TimentrisGame.h"


TimentrisGame::TimentrisGame(const string& gamePreset) : _running(false), _gameLevel(0), _isLastGameLevel(false), _score(0), _fastDescend(false) {
	_gamePresets = Configuration::getGamePreset(gamePreset);
	loadAvailablePieces();
	createCup();

	/*//Init game frame mutext
	_gameFrameMutexPtr = new mutex();*/
}


TimentrisGame::~TimentrisGame(void) {
	_running = false;

	//end game logic loop thread
	_gameLoopThread.detach();
	/*if (_gameLoopThreadPtr) {
		_gameLoopThreadPtr->detach();
		delete _gameLoopThreadPtr;
		_gameLoopThreadPtr = NULL;
	}*/

	/*if (_gameFrameMutexPtr) {
		delete _gameFrameMutexPtr;
		_gameFrameMutexPtr = NULL;
	}*/

	destroyCup();
	unloadAvailablePieces();
}

void TimentrisGame::lock() {
	getCupPtr()->lock();
	_gameFrameMutex.lock();
}
void TimentrisGame::unlock() {
	getCupPtr()->unlock();
	_gameFrameMutex.unlock();
}

void TimentrisGame::attachRender(RenderPtr render) {
	if (isRunning()) {
		throw new TimentrisGameExceptions::AlreadyRunning();
		return;
	}
	_renderPtr = render;
}

void TimentrisGame::run() {
	if (isRunning()) {
		throw new TimentrisGameExceptions::AlreadyRunning();
		return;
	}
	if (_renderPtr == NULL) {
		throw new TimentrisGameExceptions::NoRenderAttached();
		return;
	}
	//Launch main game thread
	_gameLoopThread = thread(gameLoop, this);
}

bool TimentrisGame::isRunning() const {
	return (bool)_running;
}

bool TimentrisGame::isPaused() const {
	return (bool)_paused;
}

void TimentrisGame::loadAvailablePieces() {
	//load pieces from xml files
	if (!boost::filesystem::exists("pieces") || !boost::filesystem::is_directory("pieces"))
		throw new TimentrisGameExceptions::CantLoadPieces();

	boost::filesystem::recursive_directory_iterator it("pieces");
    boost::filesystem::recursive_directory_iterator endit;
	while(it != endit) {
		cout << "Loading Piece " << it->path() <<endl;
		if (boost::filesystem::is_regular_file(*it) && it->path().extension() == ".piece") {
			Piece* p = NULL;
			try {
				p = new Piece( it->path().string());
				if (p->getDifficultyLevel() <= _gamePresets.difficultyLevel) //load only pieces with an appropriate difficulty level
					_availablePieces.push_back(p);
			} catch (PieceExceptions::PieceException) {
				cerr << "can't load piece " << it->path().string() << endl;
			}
		}
		++it;
    }

	if (_availablePieces.size() == 0) { //if for some reason no pieces where loaded, use some minimal set	
		//Dot
		Piece* pDot = new Piece();
		*pDot << Brick(0,0,0,0);

		//4-bricks l-block on z-axis (=vertical l-block)
		Piece* pI = new Piece();
		*pI << Brick(0,0,-1,0) << Brick(0,0,0,0) << Brick(0,0,1,0) << Brick(0,0,2,0);

		//Tesseract
		Piece* pTesseract = new Piece();
		*pTesseract
			<< Brick(-1,0,-1,-1) << Brick(0,0,-1,-1) << Brick(-1,-1,-1,-1) << Brick(0,-1,-1,-1)
			<< Brick(-1,0,0,-1) << Brick(0,0,0,-1) << Brick(-1,-1,0,-1) << Brick(0,-1,0,-1)
			<< Brick(-1,0,-1,0) << Brick(0,0,-1,0) << Brick(-1,-1,-1,0) << Brick(0,-1,-1,0)
			<< Brick(-1,0,0,0) << Brick(0,0,0,0) << Brick(-1,-1,0,0) << Brick(0,-1,0,0)
			;

		//L-block on XY pane
		//----
		//-x--
		//-x--
		//-xx-
		//----
		Piece* pL = new Piece();
		*pL << Brick(-1,1,0,0) << Brick(-1,0,0,0) << Brick(-1,-1,0,0) << Brick(0,-1,0,0);

		//Reverse L-block on XY pane
		//----
		//--x-
		//--x-
		//-xx-
		//----
		Piece* prL = new Piece();
		*prL << Brick(0,1,0,0) << Brick(0,0,0,0) << Brick(-1,-1,0,0) << Brick(0,-1,0,0);

		//add elements to vector
		_availablePieces.push_back(pDot);
		_availablePieces.push_back(pI);
		_availablePieces.push_back(pTesseract);
		_availablePieces.push_back(pL);
		_availablePieces.push_back(prL);
	}
}

void TimentrisGame::unloadAvailablePieces() {
	//Iterate the vector and delete each Piece
	for(Piece* p : _availablePieces) {
		delete p;
		p = NULL;
	}
	_availablePieces.clear();
}

Piece TimentrisGame::nextPiece() const {
	long long microseconds = boost::posix_time::microsec_clock::local_time().time_of_day().total_microseconds();
	return Piece(*_availablePieces[microseconds % _availablePieces.size()]); //create a COPY of this Piece
}

void TimentrisGame::createCup() {
	//load size from presets
	_cupPtr = new Cup(_gamePresets.cupDimX, _gamePresets.cupDimY, _gamePresets.cupDimZ, _gamePresets.cupDimW);
}

void TimentrisGame::destroyCup() {
	delete _cupPtr;
}

Cup* TimentrisGame::getCupPtr() const {
	return _cupPtr;
}

void TimentrisGame::gameLoop(TimentrisGame* _this) {
	//wait until Render is initialized
	while (_this->_renderPtr == NULL || !_this->_renderPtr->isRunning())
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

	_this->_running = true;
	_this->_paused = false;
	boost::timer frameTimer; //game frame timer
	double frameTimerElapsed = 0.0f;
	double frameTimerAccumulatedNegativeDelay = 0.0f;
	unsigned long long framesCount = 0;

	while (_this->_running) {
		//If the game was paused, wait until pause will be released
		while (_this->_paused)
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		
		//restart timer
		frameTimer.restart();

		//Check for gameover
		if (_this->isGameover()) { //TODO: send gameover message to th outer world
			cout << "*** Game Over ***" << endl;
			_this->_running = false;
			break;
		}


		_this->lock(); //lock frame mutext

		//Demolish lines (if there are any lines to demolish)
		vector<unsigned int> filledLines = _this->getCupPtr()->getFilledLinesIndexes();
		_this->getCupPtr()->demolishLines(filledLines);
		if (filledLines.size() > 0) {
			//add score for demolished lines (GAME_DEMOLISH_SCORE_MODIFIER * game_level * ammount_of_demolished_bricks)
			_this->addGameScore(GAME_DEMOLISH_SCORE_MODIFIER * _this->getGameLevel() * filledLines.size() * _this->_gamePresets.cupDimX * _this->_gamePresets.cupDimY * _this->_gamePresets.cupDimW);
		}

		//does piece exists?
		if (!_this->getCupPtr()->isPieceExists()) { //if not =>
			_this->getCupPtr()->placePiece(_this->nextPiece()); //=> place new random pice
		}

		//descend
		unsigned int speed = 0;
		if (_this->_fastDescend) { //fast descend
			speed = GAME_FAST_DESCEND_FRAMES;
		} else {
			try {
				speed = _this->_gamePresets.speedAtLevels.at(_this->getGameLevel()); //out_of_range exception may thrown if speed level doesn't exist
			} catch (out_of_range) {
				cerr << "Unknown speed level " << _this->getGameLevel() << "; possibly, configuration file is corrupted" << endl;
				speed = GAME_FAST_DESCEND_FRAMES;
				_this->_fastDescend = true;
			}
		}
		//cout << speed << endl; //debug
		if (framesCount % speed == 0) {
			if (_this->getCupPtr()->doesPieceCanDescend()) { //does Pice can descend?
				//cout << "descend" << endl; //debug
				//descend
				_this->getCupPtr()->operatePiece(Cup::PieceAction::Descend);
			} else {
				//cout << "decompound" << endl; //debug
				//decompound
				_this->getCupPtr()->decompoundPiece();
				//stop fast descend
				_this->_fastDescend = false;
			}
		}


		//add score for the frame
		_this->addGameScore(GAME_FRAME_SCORE_MODIFIER * _this->getGameLevel());

		//check whether we can go to the next level
		if (!_this->_isLastGameLevel) { //did we already reach last level?
			try {
				if (_this->_score >= _this->_gamePresets.scoreForLevels.at(_this->getGameLevel() + 1)) { //did we reach a desired score count?
					_this->nextGameLevel();
				}
			} catch(out_of_range) {
				_this->_isLastGameLevel = true;
			}
		}

		_this->unlock(); //release mutext lock

		//game tick delay
		frameTimerElapsed = frameTimer.elapsed();
		if (frameTimerElapsed <= GAME_FRAME_LENGTH_SEC) {
			int wait = (double)GAME_FRAME_LENGTH_MS - (double)(frameTimerElapsed*1000) + (double)(frameTimerAccumulatedNegativeDelay*1000);
			if (wait >= 0) { //we can afford to wait also accumulatedNegativeDelay in this frame
				std::this_thread::sleep_for(std::chrono::milliseconds(wait));
				frameTimerAccumulatedNegativeDelay = 0.0f;
			} else { //we can't afford to wait full negative delay
				//try half of the negative dealy
				wait = (double)GAME_FRAME_LENGTH_MS - (double)(frameTimerElapsed*1000) + (double)(frameTimerAccumulatedNegativeDelay/2*1000);
				if (wait >= 0) { //we can afford to wait half of accumulatedNegativeDelay in this frame
					std::this_thread::sleep_for(std::chrono::milliseconds(wait));
					frameTimerAccumulatedNegativeDelay = frameTimerAccumulatedNegativeDelay/2; //half still to wait
				} else { //we can't afford to wait half of the negative delay
					wait = (double)GAME_FRAME_LENGTH_MS - (double)(frameTimerElapsed*1000);
					if (wait > 0)
						std::this_thread::sleep_for(std::chrono::milliseconds(wait));
				}
			}
		} else { //calculations took too long
			frameTimerAccumulatedNegativeDelay -= frameTimerElapsed - GAME_FRAME_LENGTH_SEC;
			if (-frameTimerAccumulatedNegativeDelay > (double)GAME_FRAME_LENGTH_SEC*1.5) //too large negative dealay - we should just drop it
				frameTimerAccumulatedNegativeDelay = 0.0f;
		}

		framesCount++; //next frame
	}
}

unsigned long long TimentrisGame::addGameScore(unsigned int score) {
	_score += score;
	return getGameScore();
}
unsigned long long TimentrisGame::getGameScore() {
	return _score;
}

unsigned int TimentrisGame::nextGameLevel() {
	_gameLevel++;
	return getGameLevel();
}
unsigned int TimentrisGame::getGameLevel() {
	return _gameLevel;
}

bool TimentrisGame::isGameover() const {
	return getCupPtr()->getNumberOfBricksInLine(_gamePresets.cupDimZ - 1) > 0;
}


TimentrisGame& operator<< (TimentrisGame &g, const Stop &m) {
	g._running = false;
	return g;
}
TimentrisGame& operator<< (TimentrisGame &g, const Pause &m) {
	g._paused = true;
	return g;
}
TimentrisGame& operator<< (TimentrisGame &g, const UnPause &m) {
	g._paused = false;
	return g;
}

TimentrisGame& operator<< (TimentrisGame &g, const DescendFaster &m) {
	g.lock(); //lock frame mutext
	if (!g._paused)
		g._fastDescend = true;
	g.unlock(); //unlock frame mutext
	return g;
}
TimentrisGame& operator<< (TimentrisGame &g, const CancelDescendFaster &m) {
	g.lock(); //lock frame mutext
	if (!g._paused)
		g._fastDescend = false;
	g.unlock(); //unlock frame mutext
	return g;
}

TimentrisGame& operator<< (TimentrisGame &g, const RotateXY &m) {
	g.lock(); //lock frame mutext
	if (g.getCupPtr()->isPieceExists() && !g._paused)
		g.getCupPtr()->operatePiece(Cup::PieceAction::RotateXY);
	g.unlock(); //unlock frame mutext
	return g;
}
TimentrisGame& operator<< (TimentrisGame &g, const RotateXZ &m) {
	g.lock(); //lock frame mutext
	if (g.getCupPtr()->isPieceExists() && !g._paused)
		g.getCupPtr()->operatePiece(Cup::PieceAction::RotateXZ);
	g.unlock(); //unlock frame mutext
	return g;
}
TimentrisGame& operator<< (TimentrisGame &g, const RotateXW &m) {
	g.lock(); //lock frame mutext
	if (g.getCupPtr()->isPieceExists() && !g._paused)
		g.getCupPtr()->operatePiece(Cup::PieceAction::RotateXW);
	g.unlock(); //unlock frame mutext
	return g;
}
TimentrisGame& operator<< (TimentrisGame &g, const RotateYZ &m) {
	g.lock(); //lock frame mutext
	if (g.getCupPtr()->isPieceExists() && !g._paused)
		g.getCupPtr()->operatePiece(Cup::PieceAction::RotateYZ);
	g.unlock(); //unlock frame mutext
	return g;
}
TimentrisGame& operator<< (TimentrisGame &g, const RotateYW &m) {
	g.lock(); //lock frame mutext
	if (g.getCupPtr()->isPieceExists() && !g._paused)
		g.getCupPtr()->operatePiece(Cup::PieceAction::RotateYW);
	g.unlock(); //unlock frame mutext
	return g;
}
TimentrisGame& operator<< (TimentrisGame &g, const RotateZW &m) {
	g.lock(); //lock frame mutext
	if (g.getCupPtr()->isPieceExists() && !g._paused)
		g.getCupPtr()->operatePiece(Cup::PieceAction::RotateZW);
	g.unlock(); //unlock frame mutext
	return g;
}

TimentrisGame& operator<< (TimentrisGame &g, const MoveX1 &m) {
	g.lock(); //lock frame mutext
	if (g.getCupPtr()->isPieceExists() && !g._paused)
		g.getCupPtr()->operatePiece(Cup::PieceAction::MoveX1);
	g.unlock(); //unlock frame mutext
	return g;
}
TimentrisGame& operator<< (TimentrisGame &g, const MoveXMinus1 &m) {
	g.lock(); //lock frame mutext
	if (g.getCupPtr()->isPieceExists() && !g._paused)
		g.getCupPtr()->operatePiece(Cup::PieceAction::MoveXMinus1);
	g.unlock(); //unlock frame mutext
	return g;
}
TimentrisGame& operator<< (TimentrisGame &g, const MoveY1 &m) {
	g.lock(); //lock frame mutext
	if (g.getCupPtr()->isPieceExists() && !g._paused)
		g.getCupPtr()->operatePiece(Cup::PieceAction::MoveY1);
	g.unlock(); //unlock frame mutext
	return g;
}
TimentrisGame& operator<< (TimentrisGame &g, const MoveYMinus1 &m) {
	g.lock(); //lock frame mutext
	if (g.getCupPtr()->isPieceExists() && !g._paused)
		g.getCupPtr()->operatePiece(Cup::PieceAction::MoveYMinus1);
	g.unlock(); //unlock frame mutext
	return g;
}
TimentrisGame& operator<< (TimentrisGame &g, const MoveW1 &m) {
	g.lock(); //lock frame mutext
	if (g.getCupPtr()->isPieceExists() && !g._paused)
		g.getCupPtr()->operatePiece(Cup::PieceAction::MoveW1);
	g.unlock(); //unlock frame mutext
	return g;
}
TimentrisGame& operator<< (TimentrisGame &g, const MoveWMinus1 &m) {
	g.lock(); //lock frame mutext
	if (g.getCupPtr()->isPieceExists() && !g._paused)
		g.getCupPtr()->operatePiece(Cup::PieceAction::MoveWMinus1);
	g.unlock(); //unlock frame mutext
	return g;
}