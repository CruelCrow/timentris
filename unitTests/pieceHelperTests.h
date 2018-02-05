#include "unitTests.h"
#include "PieceHelper.h"

BOOST_AUTO_TEST_SUITE(piece_helper_test)
	
BOOST_AUTO_TEST_CASE(bricksToMatrix) {
	Brick b1;
	Brick b2(1,0,0,0);
	Brick b3(2,0,0,0);
	Brick b4(3,0,0,0);
	vector<Brick> vb1;
	vb1.push_back(b1);
	vb1.push_back(b2);
	vb1.push_back(b3);
	vb1.push_back(b4);
	Mat<int> m1 = PieceHelper::bricksToMatrix(vb1);
	Mat<int> m2(5,4);
	m2
		<< 0 << 1 << 2 << 3 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 1 << 1 << 1 << 1 << endr;
	bool isEq = true;
	if (m1.n_cols != m2.n_cols || m1.n_rows != m2.n_rows) {
		isEq = false;
	} else {
		for (unsigned int i = 0; i < m1.n_rows; i++) {
			for (unsigned int j = 0; j < m1.n_cols; j++) {
				if (m1(i,j) != m2(i,j)) {
					isEq = false;
					break;
				}
			}
		}
	}
	BOOST_CHECK_EQUAL(isEq, true);

	vector<Brick> vb2;
	BOOST_CHECK_THROW(PieceHelper::bricksToMatrix(vb2), BrickExceptions::EmptyVectorOfBricks);
	BOOST_CHECK_NO_THROW(PieceHelper::bricksToMatrix(vb1));
}
	
BOOST_AUTO_TEST_CASE(matrixToBricks) {
	Brick b1;
	Brick b2(1,0,0,0);
	Brick b3(2,0,0,0);
	Brick b4(3,0,0,0);
	vector<Brick> vb1;
	vb1.push_back(b1);
	vb1.push_back(b2);
	vb1.push_back(b3);
	vb1.push_back(b4);

	Mat<int> m1(5,4);
	m1
		<< 0 << 1 << 2 << 3 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 1 << 1 << 1 << 1 << endr;
	BOOST_CHECK_EQUAL(vb1 == PieceHelper::matrixToBricks(m1), true);

	Mat<int> m2(5,4);
	m2
		<< 0 << 1 << 2 << 3 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 13 << 0 << 1 << 666 << endr;
	BOOST_CHECK_EQUAL(vb1 == PieceHelper::matrixToBricks(m2), true);

	Mat<int> m3(5,4);
	m3
		<< 9 << 1 << 2 << 3 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 1 << 1 << 1 << 1 << endr;
	BOOST_CHECK_EQUAL(vb1 != PieceHelper::matrixToBricks(m3), true);

	Mat<int> m4(4,4);
	m4
		<< 0 << 1 << 2 << 3 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 0 << 0 << 0 << 0 << endr;
	BOOST_CHECK_EQUAL(vb1 == PieceHelper::matrixToBricks(m4), true);

	Mat<int> m5(4,4);
	m5
		<< 9 << 1 << 2 << 3 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 0 << 0 << 0 << 0 << endr;
	BOOST_CHECK_EQUAL(vb1 != PieceHelper::matrixToBricks(m5), true);

	Mat<int> m6(3,3);
	m6
		<< 9 << 1 << 2 << endr
		<< 0 << 0 << 0 << endr
		<< 0 << 0 << 0 << endr;
	BOOST_CHECK_THROW(PieceHelper::matrixToBricks(m6), BrickExceptions::WrongMatrixSize);

	Mat<int> m7(4,4);
	m7
		<< 9 << 1 << 2 << 3 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 0 << 0 << 0 << 0 << endr;
	BOOST_CHECK_NO_THROW(PieceHelper::matrixToBricks(m7));

	Mat<int> m8(4,4);
	m8
		<< 0 << 1 << 2 << 3 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 1 << 1 << 1 << 1 << endr;
	BOOST_CHECK_NO_THROW(PieceHelper::matrixToBricks(m8));
}

BOOST_AUTO_TEST_CASE(bricks_compare) {
	//matrices
	Mat<int> m1(5,4);
	m1
		<< 0 << 1 << 2 << 3 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 1 << 1 << 1 << 1 << endr;
	Mat<int> m2(5,4);
	m2
		<< 0 << 1 << 2 << 3 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 1 << 1 << 1 << 1 << endr;
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksMatrices(m1, m2), true);
	Mat<int> m3(5,4);
	m3
		<< 0 << 1 << 2 << 3 << endr
		<< 0 << 1 << 0 << 0 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 1 << 1 << 1 << 1 << endr;
	BOOST_CHECK_EQUAL(!PieceHelper::comapreBricksMatrices(m1, m3), true);

	//Compare two different matrices with the smae SET of columns
	Mat<int> m4(5,4);
	m4
		<< 0 << 2 << 1 << 3 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 1 << 1 << 1 << 1 << endr;
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksMatrices(m1, m4), true);

	//Matrix with 4 rows VS 5 rows
	Mat<int> m5(4,4);
	m5
		<< 0 << 2 << 1 << 3 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 0 << 0 << 0 << 0 << endr
		<< 0 << 0 << 0 << 0 << endr;
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksMatrices(m1, m5), true);

	//bricks
	Brick b1(0,0,0,0);
	Brick b2(1,0,0,0);
	Brick b3(2,0,0,0);
	Brick b4(3,0,0,0);
	vector<Brick> vb1;
	vb1.push_back(b1);
	vb1.push_back(b2);
	vb1.push_back(b3);
	vb1.push_back(b4);
	
	vector<Brick> vb2;
	vb2.push_back(b2);
	vb2.push_back(b3);
	vb2.push_back(b4);
	
	BOOST_CHECK_EQUAL(!PieceHelper::comapreBricksVectors(vb1, vb2), true);
	vb2.push_back(b1);
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(vb1, vb2), true);
}

BOOST_AUTO_TEST_SUITE_END( )