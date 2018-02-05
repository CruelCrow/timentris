#include "unitTests.h"
#include "Brick.h"

BOOST_AUTO_TEST_SUITE(brick_test)
	
BOOST_AUTO_TEST_CASE(get_set) {
	BOOST_CHECK_NO_THROW(
		Brick b1;
		BOOST_CHECK_EQUAL(b1.getX(), 0);
		BOOST_CHECK_EQUAL(b1.getY(), 0);
		BOOST_CHECK_EQUAL(b1.getZ(), 0);
		BOOST_CHECK_EQUAL(b1.getW(), 0);
		b1.setX(1);
		b1.setY(2);
		b1.setZ(3);
		b1.setW(4);
		BOOST_CHECK_EQUAL(b1.getX(), 1);
		BOOST_CHECK_EQUAL(b1.getY(), 2);
		BOOST_CHECK_EQUAL(b1.getZ(), 3);
		BOOST_CHECK_EQUAL(b1.getW(), 4);
	)
}

BOOST_AUTO_TEST_CASE(brackets) {
	Brick b1;
	BOOST_CHECK_NO_THROW(
		b1['x'] = 5;
		b1['y'] = 6;
		b1['z'] = 7;
		b1['w'] = 8;
		BOOST_CHECK_EQUAL(b1['x'], 5);
		BOOST_CHECK_EQUAL(b1['y'], 6);
		BOOST_CHECK_EQUAL(b1['z'], 7);
		BOOST_CHECK_EQUAL(b1['w'], 8);
		BOOST_CHECK_EQUAL(b1.getX(), b1['x']);
		BOOST_CHECK_EQUAL(b1.getY(), b1['y']);
		BOOST_CHECK_EQUAL(b1.getZ(), b1['z']);
		BOOST_CHECK_EQUAL(b1.getW(), b1['w']);
	);

	BOOST_CHECK_THROW(b1['u'] = 9, BrickExceptions::WrongCoordianteAlias);
	BOOST_CHECK_THROW(int test = b1['u'], BrickExceptions::WrongCoordianteAlias);
	BOOST_CHECK_THROW(int test = b1['o'], BrickExceptions::WrongCoordianteAlias);
}

BOOST_AUTO_TEST_CASE(eq) {
	BOOST_CHECK_NO_THROW(
		Brick b1(5,6,7,8);
		Brick b2(5,6,7,8);
		Brick b3(5,6,7,9);
		BOOST_CHECK_EQUAL(b1==b2, true);
		BOOST_CHECK_EQUAL(b1!=b3, true);
	)
}

BOOST_AUTO_TEST_CASE(assigment) {
	Mat<int> m1;
	m1
		<< 1 << endr
		<< 2 << endr
		<< 3 << endr
		<< 4 << endr
		<< 1 << endr; //fifth, not important value
	Brick b1;
	BOOST_CHECK_NO_THROW(
		b1 = m1;
	);
	BOOST_CHECK_NO_THROW(
		BOOST_CHECK_EQUAL(b1.getX(), 1);
		BOOST_CHECK_EQUAL(b1.getY(), 2);
		BOOST_CHECK_EQUAL(b1.getZ(), 3);
		BOOST_CHECK_EQUAL(b1.getW(), 4);
		BOOST_CHECK_EQUAL(b1['x'], 1);
		BOOST_CHECK_EQUAL(b1['y'], 2);
		BOOST_CHECK_EQUAL(b1['z'], 3);
		BOOST_CHECK_EQUAL(b1['w'], 4);
	);
	Brick b2;
	Mat<int> m2;
	m2
		<< 1 << endr
		<< 2 << endr
		<< 3 << endr
		<< 4 << endr;
	BOOST_CHECK_NO_THROW(
		b2 = m2;
	);
	BOOST_CHECK_NO_THROW(
		BOOST_CHECK_EQUAL(b2.getX(), 1);
		BOOST_CHECK_EQUAL(b2.getY(), 2);
		BOOST_CHECK_EQUAL(b2.getZ(), 3);
		BOOST_CHECK_EQUAL(b2.getW(), 4);
		BOOST_CHECK_EQUAL(b2['x'], 1);
		BOOST_CHECK_EQUAL(b2['y'], 2);
		BOOST_CHECK_EQUAL(b2['z'], 3);
		BOOST_CHECK_EQUAL(b2['w'], 4);
	);
	BOOST_CHECK_NO_THROW(
		BOOST_CHECK_EQUAL(b1==b2, true);
	);

	//bad cases
	Mat<int> m3;
	m3
		<< 1 << endr
		<< 2 << endr
		<< 3 << endr;
	Brick badBrick;
	BOOST_CHECK_THROW(badBrick = m3, BrickExceptions::WrongMatrixSize);
	Mat<int> m4;
	m4
		<< 1 << endr
		<< 2 << endr
		<< 3 << endr
		<< 3 << endr
		<< 3 << endr
		<< 3 << endr;
	BOOST_CHECK_THROW(badBrick = m4, BrickExceptions::WrongMatrixSize);
	Mat<int> m5;
	BOOST_CHECK_THROW(badBrick = m5, BrickExceptions::WrongMatrixSize);
	Mat<int> m6;
	m6
		<< 1 << 1 << 1 << endr
		<< 2 << 1 << 1 << endr
		<< 3 << 1 << 1 << endr
		<< 3 << 1 << 1 << endr
		<< 3 << 1 << 1 << endr
		<< 3 << 1 << 1 << endr;
	BOOST_CHECK_THROW(badBrick = m6, BrickExceptions::WrongMatrixSize);
}

BOOST_AUTO_TEST_CASE(matrix_constructor) {
	Mat<int> m1;
	m1
		<< 1 << endr
		<< 2 << endr
		<< 3 << endr
		<< 4 << endr
		<< 1 << endr; //fifth, not important value
	BOOST_CHECK_NO_THROW(Brick b1(m1));

	//bad matrix
	Mat<int> m2;
	m2
		<< 1 << 1 << 1 << endr
		<< 2 << 1 << 1 << endr
		<< 3 << 1 << 1 << endr
		<< 3 << 1 << 1 << endr
		<< 3 << 1 << 1 << endr
		<< 3 << 1 << 1 << endr;
	BOOST_CHECK_THROW(Brick bBad(m2), BrickExceptions::WrongMatrixSize);
}

BOOST_AUTO_TEST_CASE(cast_to_matrix) {
	Brick b1(5,6,7,8);
	Mat<int> m1;
	BOOST_CHECK_NO_THROW(m1 = (Mat<int>)b1);
	Mat<int> m2;
	m2
		<< 5 << endr
		<< 6 << endr
		<< 7 << endr
		<< 8 << endr
		<< 1 << endr;
	BOOST_CHECK_EQUAL(m1.n_cols == m2.n_cols && m1.n_rows == m2.n_rows && m1(0,0) == m2(0,0) && m1(1,0) == m2(1,0) && m1(2,0) == m2(2,0) && m1(3,0) == m2(3,0) && m1(4,0) == m2(4,0), true);
}

BOOST_AUTO_TEST_SUITE_END( )