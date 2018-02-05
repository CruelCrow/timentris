#include "Cup.h"


Cup::Cup(unsigned int sizeX, unsigned int sizeY, unsigned int sizeZ, unsigned int sizeW) : _pieceExists(false), _demolishedBricks(0) { //must ininitialize a pointer to NULL
	removePiece(); //no Piece at start
	removeAllBricks(); //no bricks at start
	setSizeX(sizeX);
	setSizeY(sizeY);
	setSizeZ(sizeZ);
	setSizeW(sizeW);
}

Cup::Cup(const Cup& other) : _pieceExists(other._pieceExists), _currentPiece(other._currentPiece), _sizeX(other._sizeX), _sizeY(other._sizeY), _sizeZ(other._sizeZ), _sizeW(other._sizeW), _brciks(other._brciks), _demolishedBricks(other._demolishedBricks) {
	if (this == &other) {
		return;
	}
}

Cup::~Cup(void) {
}

void Cup::lock() {
	GameObject::lock();
	_currentPiece.lock();
}
void Cup::unlock() {
	GameObject::unlock();
	_currentPiece.unlock();
}

void Cup::setSizeX(unsigned int sizeX) {
	if (sizeX == 0) throw CupExceptions::WrongCupSize();
	_sizeX = sizeX;
}
void Cup::setSizeY(unsigned int sizeY) {
	if (sizeY == 0) throw CupExceptions::WrongCupSize();
	_sizeY = sizeY;
}
void Cup::setSizeZ(unsigned int sizeZ) {
	if (sizeZ == 0) throw CupExceptions::WrongCupSize();
	_sizeZ = sizeZ;
}
void Cup::setSizeW(unsigned int sizeW) {
	if (sizeW == 0) throw CupExceptions::WrongCupSize();
	_sizeW = sizeW;
}

unsigned int Cup::getSizeX() const {
	return _sizeX;
}
unsigned int Cup::getSizeY() const {
	return _sizeY;
}
unsigned int Cup::getSizeZ() const {
	return _sizeZ;
}
unsigned int Cup::getSizeW() const {
	return _sizeW;
}

void Cup::addBrick(Brick brick) {
	//if brick exists => throw exception
	if (isBrickExists(brick)) {
		throw CupExceptions::BrickAlreadyExists();
	}
	_brciks.push_back(brick); //original brick passed as a parameter will be COPIED
}
void Cup::addBrick(unsigned int x, unsigned int y, unsigned int z, unsigned int w) {
	addBrick(Brick(x, y, z, w));
}

void Cup::removeBrick(const Brick &brick) {
	//if brick does not exists => throw exception
	if (!isBrickExists(brick)) {		
		throw CupExceptions::BrickNotFound();
	}
	for (unsigned int i = 0; i < _brciks.size(); i++) {
		if (_brciks[i] == brick) {
			//remove brick
			_brciks.erase(_brciks.begin() + i);
			return;
		}
	}
	return;
}
void Cup::removeBrick(unsigned int x, unsigned int y, unsigned int z, unsigned int w) {
	removeBrick(Brick(x, y, z, w));
}

void Cup::removeAllBricks() {
	_brciks.clear();
}

void Cup::demolishBrick(const Brick &brick) {
	removeBrick(brick);
	_demolishedBricks++; //increase number of demolished bricks
}
void Cup::demolishBrick(unsigned int x, unsigned int y, unsigned int z, unsigned int w) {
	demolishBrick(Brick(x, y, z, w));
}

void Cup::demolishAllBricks() {
	_demolishedBricks += getNumberOfBricks(); //increase number of demolished bricks
	removeAllBricks(); //now remove all of them
}

const vector<Brick>& Cup::getCupBricks() const {
	return _brciks;
}

const vector<Brick> Cup::getAllBricks() const {
	vector<Brick> allBricks(getCupBricks()); //copy Cup bricks
	//now add Piece (transformed) bricks
	if (isPieceExists()) {
		vector<Brick> pieceBricks = getPiece().getTransformedBricks();
		allBricks.insert(allBricks.end(), pieceBricks.begin(), pieceBricks.end());
	}
	return allBricks;
}

unsigned int Cup::getNumberOfDemolishedBricks() const {
	return _demolishedBricks;
}

bool Cup::isBrickExists(const Brick &brick) const {
	//if <0 or >max Cup coord
	if (
		brick.getX() < 0 || brick.getY() < 0 || brick.getZ() < 0 || brick.getW() < 0
		||
		brick.getX() > getSizeX()-1 || brick.getY() > getSizeY()-1 || brick.getZ() > getSizeZ()-1 || brick.getW() > getSizeW()-1
		) {
			throw CupExceptions::WrongBrickPosition();
	}
	for (unsigned int i = 0; i < _brciks.size(); i++) {
		if (_brciks[i] == brick) {
			return true;
		}
	}
	return false;
}
bool Cup::isBrickExists(unsigned int x, unsigned int y, unsigned int z, unsigned int w) const {
	return isBrickExists(Brick(x, y, z, w));
}

unsigned int Cup::getNumberOfBricks() const {
	return _brciks.size();
}

Piece& Cup::getPiece() const {
	//if no peace, throw exception
	if (!isPieceExists()) {
		throw CupExceptions::PieceDoesNotExists();
	}
	return (Piece&)_currentPiece;
}

bool Cup::isPieceExists() const {
	return _pieceExists;
}

bool Cup::operatePiece(PieceAction action) {
	if (!isPieceExists())
		return false;

	Piece& p = getPiece();

	switch(action) {
	case PieceAction::RotateXY:
		p.rotateXY();
		break;
	case PieceAction::RotateXZ:
		p.rotateXZ();
		break;
	case PieceAction::RotateXW:
		p.rotateXW();
		break;
	case PieceAction::RotateYZ:
		p.rotateYZ();
		break;
	case PieceAction::RotateYW:
		p.rotateYW();
		break;
	case PieceAction::RotateZW:
		p.rotateZW();
		break;
	case PieceAction::MoveX1:
		p.moveX(1);
		break;
	case PieceAction::MoveXMinus1:
		p.moveX(-1);
		break;
	case PieceAction::MoveY1:
		p.moveY(1);
		break;
	case PieceAction::MoveYMinus1:
		p.moveY(-1);
		break;
	case PieceAction::MoveW1:
		p.moveW(1);
		break;
	case PieceAction::MoveWMinus1:
		p.moveW(-1);
		break;
	case PieceAction::Descend:
		p.moveZ(-1);
		break;
	}

	bool actionOk = true;

	//Check whether the Piece is within the Cup
	Piece::metrics m = p.getTransformedMetrics();
	if (
		m.minX < 0 || m.maxX > getSizeX()-1
		||
		m.minY < 0 || m.maxY > getSizeY()-1
		||
		m.minZ < 0 || m.maxZ > getSizeZ()-1
		||
		m.minW < 0 || m.maxW > getSizeW()-1
		) {
			actionOk = false;
	}

	//Check whether the Piece took place of some Cup bricks
	if (actionOk) {
		vector<Brick> pieceBricks = p.getTransformedBricks();
		for (auto &b : pieceBricks) {
			if (isBrickExists(b)) { //duplicate brick found
				actionOk = false;
				break;
			}
		}
	}

	if (actionOk)
		return true;

	//Revert action
	switch(action) {
	case PieceAction::RotateXY:
		p.rotate(-90);
		break;
	case PieceAction::RotateXZ:
		p.rotate(0, -90);
		break;
	case PieceAction::RotateXW:
		p.rotate(0, 0, -90);
		break;
	case PieceAction::RotateYZ:
		p.rotate(0, 0, 0, -90);
		break;
	case PieceAction::RotateYW:
		p.rotate(0, 0, 0, 0, -90);
		break;
	case PieceAction::RotateZW:
		p.rotate(0, 0, 0, 0, 0, -90);
		break;
	case PieceAction::MoveX1:
		p.moveX(-1);
		break;
	case PieceAction::MoveXMinus1:
		p.moveX(1);
		break;
	case PieceAction::MoveY1:
		p.moveY(-1);
		break;
	case PieceAction::MoveYMinus1:
		p.moveY(1);
		break;
	case PieceAction::MoveW1:
		p.moveW(-1);
		break;
	case PieceAction::MoveWMinus1:
		p.moveW(1);
		break;
	case PieceAction::Descend:
		p.moveZ(1);
		break;
	}
	return false;
}

void Cup::placePiece(const Piece& pieceToCopy) {
	removePiece(); //delete the old one
	_currentPiece = pieceToCopy; //COPY that new Piece
	//Now we should place that piece at the top (max Z) on center (x,y,w);
	//In other words, we should perform set of moves for that piece.
	Piece::metrics initialMetrics = _currentPiece.getTransformedMetrics(); //piece metrics after an initail normalization (centering to O)
	_currentPiece.moveZ(getSizeZ() - 1 - initialMetrics.maxZ); //move piece to top when the highest piece brick will be at the top of the cup
	//The piece is already centered round it's own axes, so we just need to center it inside our cup
	_currentPiece.moveX(getSizeX() / 2);
	_currentPiece.moveY(getSizeY() / 2);
	_currentPiece.moveW(getSizeW() / 2);
	_pieceExists = true;
}

void Cup::removePiece() {
	_pieceExists = false;
}

void Cup::decompoundPiece() {
	if (!isPieceExists()) { //if no piece => do nothing
		return;
	}
	vector<Brick> bricksToPlace = getPiece().getTransformedBricks();
	//Now we have bricks of the piece on their dedicated positions (relative to the up, of course);
	//So we can delete the piece and continue to work with bircks.
	removePiece();
	//Add piece bricks to the cup
	for (unsigned int i = 0; i < bricksToPlace.size(); i++) {
		//Place new brick only if it does not exists yet in the cup... just in case, for an extreme cases
		if (!isBrickExists(bricksToPlace[i])) {
			addBrick(bricksToPlace[i]);
		}
	}
}

bool Cup::doesPieceCanDescend() const {
	if (!isPieceExists()) { //if no piece => can't descend
		return false;
	}
	Piece::metrics pieceMetrics = getPiece().getTransformedMetrics();
	if (pieceMetrics.minZ <= 0) {
		return false;
	}
	//Piece::metrics does not help... so must check each piece brick separately
	vector<Brick> pieceBricks = getPiece().getTransformedBricks();
	for (unsigned int i = 0; i < pieceBricks.size(); i++) { //for each piece brick check whether there is a brick in the cup right down this piece brick
		if (isBrickExists(pieceBricks[i]['x'], pieceBricks[i]['y'], pieceBricks[i]['z'] - 1, pieceBricks[i]['w'])) {
			return false;
		}
	}
	return true;
}

unsigned int Cup::getNumberOfBricksInLine(const unsigned int lineNumber) const {
	unsigned int actualNumOfBricksInLine = 0;
	for (unsigned int i = 0; i < _brciks.size(); i++) {
		if ((unsigned int)(_brciks[i].getZ()) == lineNumber) {
			actualNumOfBricksInLine++;
		}
	}
	return actualNumOfBricksInLine;
}

bool Cup::isLineFilled(const unsigned int lineNumber) const {
	unsigned int numOfBricksInLine = getSizeX() * getSizeY() * getSizeW();
	unsigned int actualNumOfBricksInLine = getNumberOfBricksInLine(lineNumber);
	if (actualNumOfBricksInLine > numOfBricksInLine) { //TOO MUCH!!!
		//Can't be!
		//Throw exception!
		throw CupExceptions::DuplicateBricks();
	} else if (actualNumOfBricksInLine == numOfBricksInLine) { //exactl ammount of bricks
		return true;
	} else { //to few bricks
		return false;
	}
}

vector<unsigned int> Cup::getFilledLinesIndexes() const {
	vector<unsigned int> filledLinesIndexes;
	for (unsigned int i = 0; i < getSizeZ() - 1; i++) {
		if (isLineFilled(i)) {
			filledLinesIndexes.push_back(i);
		}
	}
	return filledLinesIndexes;
}

void Cup::demolishLine(const unsigned int lineIndex) {
	for (unsigned int i = 0; i < _brciks.size(); i++) {
		if ((unsigned int)(_brciks[i].getZ()) == lineIndex) {
			demolishBrick(_brciks[i--]); //decrement index because size of the vector will change
		}
	}
	//now descend ALL bricks on top of that line by 1
	for (unsigned int j = 0; j < _brciks.size(); j++) { //loop for all bricks
		if ((unsigned int)(_brciks[j].getZ()) > lineIndex) { //that brick is above lineIndex
			_brciks[j].setZ(_brciks[j].getZ() - 1); //descend
		}
	}
}

void Cup::demolishLines(const vector<unsigned int>& lineIndexes) {
	unsigned int lineToDemolish;
	unsigned int demolishedLinesCount = 0;
	for (unsigned int i = 0; i < lineIndexes.size(); i++) {
		lineToDemolish = lineIndexes[i] - demolishedLinesCount; //!Take in account that when line is demolished, all lines above it will descend (z-1)
		demolishLine(lineToDemolish);
		demolishedLinesCount++; //one more lines was demolished
	}
}

vector<Cup3dSlice> Cup::get3dSlices() const {
	vector<Cup3dSlice> slices; //return value
	slices.reserve(getSizeW());
	const vector<Brick>& cupBricks = getCupBricks();
	vector<Brick> pieceBricks;
	if (isPieceExists()) {
		pieceBricks = getPiece().getTransformedBricks();
	}

	for (unsigned int w = 0; w < getSizeW(); w++) { //for each W-slice
		Cup3dSlice currentSlice(getSizeX(), getSizeY(), getSizeZ()); //a new empty projection
		//chose all bricks which have this W coordinate

		for (unsigned int j = 0; j < cupBricks.size(); j++) { //cup bricks
			if (cupBricks[j]['w'] == w) { //such brick found
				currentSlice._grid(cupBricks[j]['x'], cupBricks[j]['y'], cupBricks[j]['z']) = GRIDCELL_CUP_BRICK;
			}
		}

		for (unsigned int j = 0; j < pieceBricks.size(); j++) { //and now piece bricks
			if (pieceBricks[j]['w'] == w) { //such brick found
				currentSlice._grid(pieceBricks[j]['x'], pieceBricks[j]['y'], pieceBricks[j]['z']) = GRIDCELL_PIECE_BRICK;
			}
		}

		//now add this slice to the slices vector
		slices.push_back(currentSlice);
	}

	return slices;
}

array<Cup3dSubspace, 4> Cup::get3dOrtonormalSubspaces() const {
	array<Cup3dSubspace, 4> subspaces;
	const vector<Brick>& cupBricks = getCupBricks();
	vector<Brick> pieceBricks;
	if (isPieceExists()) {
		pieceBricks = getPiece().getTransformedBricks();
	}

	//Create 4 subspaces
	subspaces[SUBSPACE_XYZ] = Cup3dSubspace(getSizeX(), getSizeY(), getSizeZ());
	subspaces[SUBSPACE_XYW] = Cup3dSubspace(getSizeX(), getSizeY(), getSizeW());
	subspaces[SUBSPACE_XZW] = Cup3dSubspace(getSizeX(), getSizeZ(), getSizeW());
	subspaces[SUBSPACE_YZW] = Cup3dSubspace(getSizeY(), getSizeZ(), getSizeW());

	//place bricks into subspaces
	for (unsigned int i = 0; i < cupBricks.size(); i++) { //cup bricks
		subspaces[SUBSPACE_XYZ]._grid(cupBricks[i]['x'], cupBricks[i]['y'], cupBricks[i]['z']) = GRIDCELL_CUP_BRICK;
		subspaces[SUBSPACE_XYW]._grid(cupBricks[i]['x'], cupBricks[i]['y'], cupBricks[i]['w']) = GRIDCELL_CUP_BRICK;
		subspaces[SUBSPACE_XZW]._grid(cupBricks[i]['x'], cupBricks[i]['z'], cupBricks[i]['w']) = GRIDCELL_CUP_BRICK;
		subspaces[SUBSPACE_YZW]._grid(cupBricks[i]['y'], cupBricks[i]['z'], cupBricks[i]['w']) = GRIDCELL_CUP_BRICK;
	}

	for (unsigned int i = 0; i < pieceBricks.size(); i++) { //piece bricks
		subspaces[SUBSPACE_XYZ]._grid(pieceBricks[i]['x'], pieceBricks[i]['y'], pieceBricks[i]['z']) = GRIDCELL_PIECE_BRICK;
		subspaces[SUBSPACE_XYW]._grid(pieceBricks[i]['x'], pieceBricks[i]['y'], pieceBricks[i]['w']) = GRIDCELL_PIECE_BRICK;
		subspaces[SUBSPACE_XZW]._grid(pieceBricks[i]['x'], pieceBricks[i]['z'], pieceBricks[i]['w']) = GRIDCELL_PIECE_BRICK;
		subspaces[SUBSPACE_YZW]._grid(pieceBricks[i]['y'], pieceBricks[i]['z'], pieceBricks[i]['w']) = GRIDCELL_PIECE_BRICK;
	}

	return subspaces;
}

Cup& Cup::operator=(const Cup &rhs) {
	if (this == &rhs) {
		return *this;
	}
	Cup temp(rhs);
	swap(_pieceExists, temp._pieceExists);
	swap(_currentPiece, temp._currentPiece);
	swap(_sizeX, temp._sizeX);
	swap(_sizeY, temp._sizeY);
	swap(_sizeZ, temp._sizeZ);
	swap(_sizeW, temp._sizeW);
	swap(_demolishedBricks, temp._demolishedBricks);
	swap(_brciks, temp._brciks);
	return *this;
}

Cup& operator<< (Cup &c, const Piece &p) {
	c.placePiece(p);
	return c;
}

Cup& operator<< (Cup &c, const Brick &b) {
	c.addBrick(b);
	return c;
}

Cup& operator>> (Cup &c, vector<Cup3dSlice> &slices) {
	slices = c.get3dSlices();
	return c;
}

Cup& operator>> (Cup &c, array<Cup3dSubspace, 4> &subspaces) {
	subspaces = c.get3dOrtonormalSubspaces();
	return c;
}