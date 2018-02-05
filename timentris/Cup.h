#pragma once
#include <memory>
#include <vector>
#include <array>
#include <algorithm>
#include "exceptions.h"
#include "GameObject.h"
#include "Brick.h"
#include "Piece.h"
#include "Cup3dProjection.h"

//A fully-functionall 4D Cup;
//Please note that there is no negative coordinates;
//Z=0 means bottom, Z=_sizeZ-1 means top
class Cup : public GameObject {
public:
	enum class PieceAction {
		RotateXY,
		RotateXZ,
		RotateXW,
		RotateYZ,
		RotateYW,
		RotateZW,
		MoveX1,
		MoveXMinus1,
		MoveY1,
		MoveYMinus1,
		MoveW1,
		MoveWMinus1,
		Descend
	};
private:
	unsigned int _sizeX;
	unsigned int _sizeY;
	unsigned int _sizeZ; //HEIGHT
	unsigned int _sizeW;

	bool _pieceExists;
	Piece _currentPiece;
	vector<Brick> _brciks;

	unsigned int _demolishedBricks; //count of a demolished bricks
public:
	Cup(unsigned int sizeX, unsigned int sizeY, unsigned int sizeZ, unsigned int sizeW);
	Cup(const Cup& other); //copy constructor
	~Cup(void);

	void lock();
	void unlock();

private:
	void setSizeX(unsigned int sizeX);
	void setSizeY(unsigned int sizeY);
	void setSizeZ(unsigned int sizeZ);
	void setSizeW(unsigned int sizeW);
	
public:
	unsigned int getSizeX() const;
	unsigned int getSizeY() const;
	unsigned int getSizeZ() const;
	unsigned int getSizeW() const;

	void addBrick(Brick brick); //copies the brick and puts it into _bricks
	void addBrick(unsigned int x, unsigned int y, unsigned int z, unsigned int w);
	
	//only for a technical use (like creating a level), DO NOT use this group of method for lines demolishing
	void removeBrick(const Brick &brick);
	void removeBrick(unsigned int x, unsigned int y, unsigned int z, unsigned int w);
	void removeAllBricks();

	//in-game bricks demolishing + increments a demolishedBricks count
	void demolishBrick(const Brick &brick);
	void demolishBrick(unsigned int x, unsigned int y, unsigned int z, unsigned int w);
	void demolishAllBricks();

	const vector<Brick>& getCupBricks() const; //get all bricks in the Cup, without a bricks within the Piece
	const vector<Brick> getAllBricks() const; //get all bricks in the Cup, INCLUDING a bricks within the Piece

	unsigned int getNumberOfDemolishedBricks() const;
	
	bool isBrickExists(const Brick &brick) const; //ignoring bricks within the Piece
	bool isBrickExists(unsigned int x, unsigned int y, unsigned int z, unsigned int w) const;

	unsigned int getNumberOfBricks() const; //get bricks count, WITHOUT a bricks within the Piece

	Piece& getPiece() const;
	bool isPieceExists() const;
	bool operatePiece(PieceAction action); //method which will perform check whether the Piece can move/rotate
	void placePiece(const Piece& pieceToCopy); //old _currentPiecePtr Piece will be removed; the COPY of newPiece will be stored
	void removePiece(); //just remove current Piece and do nothing more
	void decompoundPiece(); //decompound Piece into bricks which will stay in the cup until removed or demolished along with 3d (x,y,w) "line" (actually, space)

	bool doesPieceCanDescend() const; //checks whether a Z-coordinate of the Piece could be decreased any further without causing a piece to be mixed up with an existing bricks (and, of course, not to hit the ground); if there is no piece, return false

	unsigned int getNumberOfBricksInLine(const unsigned int lineNumber) const; //WARNING: if we somehow have two IDENTICAL bricks on the same line, function will return a WRONG result
	bool isLineFilled(const unsigned int lineNumber) const; //check whether the "line" (zero-based index on Z axis) if fully filled with a bricks
	vector<unsigned int> getFilledLinesIndexes() const;
	void demolishLine(const unsigned int lineIndex); //remove all bricks in the "line", than descend all bricks on top of it by 1
	void demolishLines(const vector<unsigned int>& lineIndexes);

	vector<Cup3dSlice> get3dSlices() const; //return set of 3D projections for 3D (x,y,z) "slices" of 4-dimentional space (a number of such slices will be same as _sizeW)
	array<Cup3dSubspace, 4> get3dOrtonormalSubspaces() const; //return set of 4 3D projections for 4D space ortonormal subspaces: {(1,0,0,0),(0,1,0,0),(0,0,1,0)}, {(1,0,0,0),(0,1,0,0),(0,0,0,1)}, {(1,0,0,0),(0,0,1,0),(0,0,0,1)}, {(0,1,0,0),(0,0,1,0),(0,0,0,1)} (or, (x,y,z), (x,y,w), (x,z,w), (y,z,w))
	
	Cup& operator=(const Cup &rhs); //assigner for copy constructor

	friend Cup& operator<< (Cup &c, const Piece &p); //Piece will be COPIED (equal to replacePiece())
	friend Cup& operator<< (Cup &c, const Brick &b); //Brick will be COPIED

	friend Cup& operator>> (Cup &c, vector<Cup3dSlice> &slices);
	friend Cup& operator>> (Cup &c, array<Cup3dSubspace, 4> &subspaces);
};
