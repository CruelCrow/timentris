#include "PieceHelper.h"


PieceHelper::PieceHelper(void)
{
}


PieceHelper::~PieceHelper(void)
{
}

Mat<int> PieceHelper::bricksToMatrix(const vector<Brick>& bricks) {
	if (bricks.size() < 1) {
		throw BrickExceptions::EmptyVectorOfBricks();
	}
	Mat<int> retMat(5, bricks.size());
	for (unsigned int i = 0; i < bricks.size(); i++) {
		retMat(0,i) = bricks[i].getX();
		retMat(1,i) = bricks[i].getY();
		retMat(2,i) = bricks[i].getZ();
		retMat(3,i) = bricks[i].getW();
		retMat(4,i) = 1;
	}
	return retMat;
}

vector<Brick> PieceHelper::matrixToBricks(const Mat<int>& matrix) {
	if (matrix.n_cols < 1 || (matrix.n_rows != 4 && matrix.n_rows != 5)) {
		throw BrickExceptions::WrongMatrixSize();
	}
	vector<Brick> bricks;
	for (unsigned int i = 0; i < matrix.n_cols; i++) {
		bricks.push_back(Brick(matrix(0,i), matrix(1,i), matrix(2,i), matrix(3,i)));
	}
	return bricks;
}

bool PieceHelper::comapreBricksVectors(const vector<Brick>& vb1, const vector<Brick>& vb2){
	if (vb1.size() < 1 || vb2.size() < 1) {
		throw BrickExceptions::EmptyVectorOfBricks();
	}
	if (vb1.size() != vb2.size()) {
		return false;
	}
	unsigned int i = 0;
	unsigned int j = 0;
	for (i = 0; i < vb1.size(); i++) {
		for (j = 0; j < vb2.size(); j++) {
			if (vb1[i] == vb2[j]) {
				break;
			}
		}
		if (j >= vb2.size()) { //not found
			return false;
		}
	}
	return true;
}

bool PieceHelper::comapreBricksMatrices(const Mat<int>& m1, const Mat<int>& m2) {
	//Basic checks
	if ( (m1.n_cols < 1 || (m1.n_rows != 4 && m1.n_rows != 5)) || (m2.n_cols < 1 || (m2.n_rows != 4 && m2.n_rows != 5)) ) {
		throw BrickExceptions::WrongMatrixSize();
	}
	if (m1.n_cols != m2.n_cols) { //different count of elements
		return false;
	}

	return comapreBricksVectors(matrixToBricks(m1), matrixToBricks(m2));
}
