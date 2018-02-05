#pragma once
#include <vector>
#include <armadillo>
#include "exceptions.h"
#include "Brick.h"

class PieceHelper
{
public:
	PieceHelper(void);
	~PieceHelper(void);

	static Mat<int> bricksToMatrix(const vector<Brick>& bricks); //convert vector of bricks to matrix 5*x
	static vector<Brick> matrixToBricks(const Mat<int>& matrix); //convert matrix (5 or 4)*x to vector of bricks
	static bool comapreBricksVectors(const vector<Brick>& vb1, const vector<Brick>& vb2);
	static bool comapreBricksMatrices(const Mat<int>& m1, const Mat<int>& m2);
};

