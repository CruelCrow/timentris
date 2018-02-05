#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <armadillo>
#include <tinyxml2.h>
#include "exceptions.h"
#include "GameObject.h"
#include "Brick.h"
#include "PieceHelper.h"

using namespace std;
using namespace arma;
using namespace tinyxml2;

class Piece : public GameObject {
public:
	struct metrics
	{
		int minX;
		int minY;
		int minZ;
		int minW;
		int maxX;
		int maxY;
		int maxZ;
		int maxW;
		unsigned int sizeX;
		unsigned int sizeY;
		unsigned int sizeZ;
		unsigned int sizeW;
		bool operator==(const metrics& m) {
			return 
				(
				minX == m.minX
				&& minY == m.minY
				&& minZ == m.minZ
				&& minW == m.minW
				&& maxX == m.maxX
				&& maxY == m.maxY
				&& maxZ == m.maxZ
				&& maxW == m.maxW
				&& sizeX == m.sizeX
				&& sizeY == m.sizeY
				&& sizeZ == m.sizeZ
				&& sizeW == m.sizeW
				);
		}
		bool operator!=(const metrics& m) {
			return !(*this == m);
		}
	};
	//Tmake primary static methods works with matrices and base basic static method working with pieces on them
	static metrics getMetrics(const Mat<int> &m);
	static metrics getMetrics(const vector<Brick> &vb);
	static metrics getMetrics(const Piece &p); //recieve metrics of an untouched piece (without rotations or shifts)
	static Mat<int> getNormalizedMatrix(const Mat<int> &m); //normalize (center) Piece presented as bricks matrix and return a normalized matrix
	static Mat<int> getNormalizedMatrix(const vector<Brick> &vb);
	static Mat<int> getNormalizedMatrix(const Piece &p); //normalize (center) Piece and return a normalized matrix
private:
	unsigned int _difficultyLevel;

	vector<Brick> _originalBricks; //an original array of bricks

	//shifts
	int _dX;
	int _dY;
	int _dZ;
	int _dW;

	//rotations in angles round the planes (2d-"axises") (must be executed before shifts)
	int _aXY;
	int _aXZ;
	int _aXW;
	int _aYZ;
	int _aYW;
	int _aZW;

	void init(void);
public:
	Piece(void);
	Piece(const vector<Brick> &bricks);
	Piece(const Mat<int> &mBricks);
	Piece(const string filename); //load new Piece from file

	void setDifficultyLevel(unsigned int difficultyLevel = 0);
	unsigned int getDifficultyLevel();

	void addBrick(const Brick &brick);
	void addBrick(int x, int y, int z, int w);
	
	void removeBrick(const Brick &brick);
	void removeBrick(int x, int y, int z, int w);
	void removeAllBricks();
	
	bool isBrickExists(const Brick &brick) const; //for original (not transformed bricks)
	bool isBrickExists(int x, int y, int z, int w) const;

	unsigned int getNumberOfBricks() const;

	void rotate(int aXY=0, int aXZ=0, int aXW=0, int aYZ=0, int aYW=0, int aZW=0); //rotation is possible only for %90 degrees
	void rotateXY();
	void rotateXZ();
	void rotateXW();
	void rotateYZ();
	void rotateYW();
	void rotateZW();
	void clearRotations();

	void move(int dX, int dY, int dZ, int dW);
	void moveX(int dX);
	void moveY(int dY);
	void moveZ(int dZ);
	void moveW(int dW);
	void clearShifts();

	bool saveToFile(const string filename) const; //returns TRUE on success, FALSE otherwise

private:
	Mat<int> calculateRotationMatrix() const; //MUST be called BEFORE calculateShiftMatrix()!
	Mat<int> calculateShiftMatrix() const;

public:
	metrics getOrigianlMetrics() const;
	metrics getTransformedMetrics() const;

	const vector<Brick>& getOriginalBricks() const;
	vector<Brick> getTransformedBricks() const;

	bool operator== (const Piece &p) const; //does NOT take in account rotations or shifts (including normalization shifts)
	bool operator!= (const Piece &p) const;
	friend Piece& operator<< (Piece &p, const Brick &b);
};

//TODO: move static methods (or at least, part of them) to a static helper class