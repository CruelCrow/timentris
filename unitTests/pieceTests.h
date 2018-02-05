#include "unitTests.h"
#include "Brick.h"
#include "Piece.h"

BOOST_AUTO_TEST_SUITE(piece_test)
	
BOOST_AUTO_TEST_CASE(basic) {
	//default constructor
	Piece p1;
	BOOST_CHECK_EQUAL(p1.getNumberOfBricks(), 0);
	p1.addBrick(0,0,0,0);
	p1.addBrick(1,0,0,0);
	p1.addBrick(2,0,0,0);
	BOOST_CHECK_EQUAL(p1.getNumberOfBricks(), 3);
	p1.removeBrick(0,0,0,0);
	BOOST_CHECK_EQUAL(p1.getNumberOfBricks(), 2);
	BOOST_CHECK_THROW(p1.removeBrick(0,0,0,0), PieceExceptions::BrickNotFound);
	BOOST_CHECK_EQUAL(p1.isBrickExists(0,0,0,0), false);
	BOOST_CHECK_EQUAL(p1.isBrickExists(1,0,0,0), true);
	BOOST_CHECK_EQUAL(p1.isBrickExists(2,0,0,0), true);
	BOOST_CHECK_THROW(p1.addBrick(1,0,0,0), PieceExceptions::BrickAlreadyExists);
	BOOST_CHECK_EQUAL(p1.getNumberOfBricks(), 2);

	//copy constructor
	Piece p2(p1);
	BOOST_CHECK_EQUAL(&p2 == &p1, false); //different addresses
	BOOST_CHECK_EQUAL(p2.getNumberOfBricks() != 0, true);
	BOOST_CHECK_EQUAL(p2.getNumberOfBricks(), 2);
	BOOST_CHECK_EQUAL(p2.isBrickExists(0,0,0,0), false);
	BOOST_CHECK_EQUAL(p2.isBrickExists(1,0,0,0), true);
	BOOST_CHECK_EQUAL(p2.isBrickExists(2,0,0,0), true);
	BOOST_CHECK_THROW(p2.removeBrick(0,0,0,0), PieceExceptions::BrickNotFound);
	BOOST_CHECK_THROW(p2.addBrick(1,0,0,0), PieceExceptions::BrickAlreadyExists);
	BOOST_CHECK_EQUAL(p2 == p1, true);
	BOOST_CHECK_EQUAL(!(p2 != p1), true);

	Piece p3;
	p3 = p1; //operator=
	BOOST_CHECK_EQUAL(&p3 == &p1, false); //different addresses
	BOOST_CHECK_EQUAL(p3.getNumberOfBricks() != 0, true);
	BOOST_CHECK_EQUAL(p3.getNumberOfBricks(), 2);
	BOOST_CHECK_EQUAL(p3.isBrickExists(0,0,0,0), false);
	BOOST_CHECK_EQUAL(p3.isBrickExists(1,0,0,0), true);
	BOOST_CHECK_EQUAL(p3.isBrickExists(2,0,0,0), true);
	BOOST_CHECK_THROW(p3.removeBrick(0,0,0,0), PieceExceptions::BrickNotFound);
	BOOST_CHECK_THROW(p3.addBrick(1,0,0,0), PieceExceptions::BrickAlreadyExists);
	BOOST_CHECK_EQUAL(p3 == p1, true);

	//matrix constructor
	Mat<int> m1(5,4);
	m1
		<< 0 << 1 << 2 << endr
		<< 0 << 0 << 0 << endr
		<< 0 << 0 << 0 << endr
		<< 0 << 0 << 0 << endr
		<< 1 << 1 << 1 << endr;
	Piece p4(m1);
	BOOST_CHECK_EQUAL(p4.getNumberOfBricks() != 0, true);
	p4.removeBrick(0,0,0,0);
	BOOST_CHECK_EQUAL(p4.getNumberOfBricks(), 2);
	BOOST_CHECK_EQUAL(p4.isBrickExists(0,0,0,0), false);
	BOOST_CHECK_EQUAL(p4.isBrickExists(1,0,0,0), true);
	BOOST_CHECK_EQUAL(p4.isBrickExists(2,0,0,0), true);
	BOOST_CHECK_THROW(p4.removeBrick(0,0,0,0), PieceExceptions::BrickNotFound);
	BOOST_CHECK_THROW(p4.addBrick(1,0,0,0), PieceExceptions::BrickAlreadyExists);
	BOOST_CHECK_EQUAL(p4 == p1, true);

	//default constructor with overloaded <<
	Piece p5;
	p5 << Brick(0,0,0,0) << Brick(1,0,0,0) << Brick(2,0,0,0);
	BOOST_CHECK_NO_THROW(p5.removeBrick(0,0,0,0));
	BOOST_CHECK_EQUAL(p5.getNumberOfBricks(), 2);
	BOOST_CHECK_EQUAL(p5.isBrickExists(0,0,0,0), false);
	BOOST_CHECK_EQUAL(p5.isBrickExists(1,0,0,0), true);
	BOOST_CHECK_EQUAL(p5.isBrickExists(2,0,0,0), true);
	BOOST_CHECK_THROW(p5.removeBrick(0,0,0,0), PieceExceptions::BrickNotFound);
	BOOST_CHECK_THROW(p5.addBrick(1,0,0,0), PieceExceptions::BrickAlreadyExists);
	BOOST_CHECK_EQUAL(p5 == p1, true);

	//vector constructor
	Brick b1(0,0,0,0);
	Brick b2(1,0,0,0);
	Brick b3(2,0,0,0);
	vector<Brick> vb1;
	vb1.push_back(b1);
	vb1.push_back(b2);
	vb1.push_back(b3);
	Piece p6(vb1);
	BOOST_CHECK_NO_THROW(p6.removeBrick(0,0,0,0));
	BOOST_CHECK_EQUAL(p6.getNumberOfBricks(), 2);
	BOOST_CHECK_EQUAL(p6.isBrickExists(0,0,0,0), false);
	BOOST_CHECK_EQUAL(p6.isBrickExists(1,0,0,0), true);
	BOOST_CHECK_EQUAL(p6.isBrickExists(2,0,0,0), true);
	BOOST_CHECK_THROW(p6.removeBrick(0,0,0,0), PieceExceptions::BrickNotFound);
	BOOST_CHECK_THROW(p6.addBrick(1,0,0,0), PieceExceptions::BrickAlreadyExists);
	BOOST_CHECK_EQUAL(p6 == p1, true);
}

BOOST_AUTO_TEST_CASE(metrics) {
	BOOST_CHECK_THROW(Piece::getMetrics(Mat<int>("1 0; 0 1")), PieceExceptions::WrongMatrixSize);

	Mat<int> m1(5,4);
	m1
		<< 0 << 1 << 2 << 3 << endr
		<< 0 << 1 << 4 << 3 << endr
		<< 0 << 1 << 4 << 5 << endr
		<< 0 << 5 << 4 << 5 << endr
		<< 1 << 1 << 1 << 1 << endr;
	BOOST_CHECK_NO_THROW(Piece::getMetrics(m1));
	Piece::metrics metrics1 = Piece::getMetrics(m1);

	Piece::metrics cmpMetrics;
	cmpMetrics.minX = 0;
	cmpMetrics.minY = 0;
	cmpMetrics.minZ = 0;
	cmpMetrics.minW = 0;
	cmpMetrics.maxX = 3;
	cmpMetrics.maxY = 4;
	cmpMetrics.maxZ = 5;
	cmpMetrics.maxW = 5;
	cmpMetrics.sizeX = cmpMetrics.maxX - cmpMetrics.minX + 1;
	cmpMetrics.sizeY = cmpMetrics.maxY - cmpMetrics.minY + 1;
	cmpMetrics.sizeZ = cmpMetrics.maxZ - cmpMetrics.minZ + 1;
	cmpMetrics.sizeW = cmpMetrics.maxW - cmpMetrics.minW + 1;

	BOOST_CHECK_EQUAL(metrics1 == cmpMetrics, true);
	BOOST_CHECK_EQUAL(metrics1 != cmpMetrics, false);
	
	Brick b1(0,0,0,0);
	Brick b2(1,1,1,5);
	Brick b3(2,4,4,4);
	Brick b4(3,3,5,5);
	vector<Brick> vb1;
	vb1.push_back(b1);
	vb1.push_back(b2);
	vb1.push_back(b3);
	vb1.push_back(b4);

	Piece::metrics metrics2 = Piece::getMetrics(vb1);
	BOOST_CHECK_EQUAL(metrics2 == cmpMetrics, true);

	Piece p1(vb1);
	Piece::metrics metrics3 = Piece::getMetrics(p1);
	BOOST_CHECK_EQUAL(metrics3 == cmpMetrics, true);

	Piece p2(p1);
	Piece::metrics metrics4 = Piece::getMetrics(p2);
	BOOST_CHECK_EQUAL(metrics4 == cmpMetrics, true);
}

BOOST_AUTO_TEST_CASE(static_normalization) {
	BOOST_CHECK_THROW(Piece::getNormalizedMatrix(Mat<int>("1 0; 0 1")), PieceExceptions::WrongMatrixSize);

	//5 rows vs 4 rows
	Mat<int> m1(5,4);
	m1
		<< 0 << 1 << 2 << 3 << endr
		<< 0 << 1 << 4 << 3 << endr
		<< 0 << 1 << 4 << 5 << endr
		<< 0 << 5 << 4 << 5 << endr
		<< 1 << 1 << 1 << 1 << endr;
	BOOST_CHECK_NO_THROW(Piece::getNormalizedMatrix(m1));
	Mat<int> m2(4,4);
	m2
		<< 0 << 1 << 2 << 3 << endr
		<< 0 << 1 << 4 << 3 << endr
		<< 0 << 1 << 4 << 5 << endr
		<< 0 << 5 << 4 << 5 << endr;
	BOOST_CHECK_NO_THROW(Piece::getNormalizedMatrix(m2));

	Mat<int> mNorm1 = Piece::getNormalizedMatrix(m1);
	Mat<int> mNorm2 = Piece::getNormalizedMatrix(m2);
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksMatrices(mNorm1, mNorm2), true);
	
	//TODO: static noramlizations
}

BOOST_AUTO_TEST_CASE(normaliztion) {
	Piece p1;
	p1 << Brick(0,0,0,0);
	BOOST_CHECK_NO_THROW(p1.getTransformedBricks());
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(p1.getOriginalBricks(), p1.getTransformedBricks()), true); //must be equal to original, because we have a single brick in the center

	//Another single-brick pieces
	Piece p2;
	p2 << Brick(1,0,0,0);
	BOOST_CHECK_NO_THROW(p2.getTransformedBricks());
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(p1.getTransformedBricks(), p2.getTransformedBricks()), true); //must be similar to the first piece (the only piece brick must be at the center)
	Piece p3;
	p3 << Brick(0,16,0,0);
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(p1.getTransformedBricks(), p3.getTransformedBricks()), true);
	Piece p4;
	p4 << Brick(0,0,666,0);
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(p1.getTransformedBricks(), p4.getTransformedBricks()), true);
	Piece p5;
	p5 << Brick(0,0,0,43825);
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(p1.getTransformedBricks(), p5.getTransformedBricks()), true);
	Piece p6;
	p6 << Brick(-1,0,0,0);
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(p1.getTransformedBricks(), p6.getTransformedBricks()), true);
	Piece p7;
	p7 << Brick(0,-16,0,0);
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(p1.getTransformedBricks(), p7.getTransformedBricks()), true);
	Piece p8;
	p8 << Brick(0,0,-666,0);
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(p1.getTransformedBricks(), p8.getTransformedBricks()), true);
	Piece p9;
	p9 << Brick(0,0,0,-43825);
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(p1.getTransformedBricks(), p9.getTransformedBricks()), true);

	//l-blocks
	Piece plSample1;
	plSample1 << Brick(-2,0,0,0) << Brick(-1,0,0,0) << Brick(0,0,0,0) << Brick(1,0,0,0); //4-bricks SYMETRICAL l-block on x-axis
	Piece pl1;
	pl1 << Brick(0,0,0,0) << Brick(1,0,0,0) << Brick(2,0,0,0) << Brick(3,0,0,0); //4-bricks l-block on x-axis
	Piece pl2;
	pl2 << Brick(0,10,0,0) << Brick(1,10,0,0) << Brick(2,10,0,0) << Brick(3,10,0,0); //4-bricks l-block on x-axis, but placed at 10 by y axis
	Piece pl3;
	pl3 << Brick(0,10,50,0) << Brick(1,10,50,0) << Brick(2,10,50,0) << Brick(3,10,50,0); //4-bricks l-block on x-axis, but placed at 10 by y axis and at 50 by z axis
	Piece pl4;
	pl4 << Brick(0,10,50,666) << Brick(1,10,50,666) << Brick(2,10,50,666) << Brick(3,10,50,666); //4-bricks l-block on x-axis, but placed at 10 by y axis, at 50 by z axis and at 666 by w axis
	Piece pl5;
	pl5 << Brick(20,10,50,666) << Brick(21,10,50,666) << Brick(22,10,50,666) << Brick(23,10,50,666); //4-bricks l-block on x-axis, but placed at 10 by y axis, at 50 by z axis and at 666 by w axis + moved by 20 at x axis
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(plSample1.getTransformedBricks(), pl1.getTransformedBricks()), true);
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(plSample1.getTransformedBricks(), pl2.getTransformedBricks()), true);
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(plSample1.getTransformedBricks(), pl3.getTransformedBricks()), true);
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(plSample1.getTransformedBricks(), pl4.getTransformedBricks()), true);
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(plSample1.getTransformedBricks(), pl5.getTransformedBricks()), true);

	//l-block with another orientation
	Piece plSample2;
	plSample2 << Brick(0,-2,0,0) << Brick(0,-1,0,0) << Brick(0,0,0,0) << Brick(0,1,0,0); //4-bricks SYMETRICAL l-block on y-axis
	Piece pl6;
	pl6 << Brick(-40,-100,80,90) << Brick(-40,-99,80,90) << Brick(-40,-98,80,90) << Brick(-40,-97,80,90);
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(plSample2.getTransformedBricks(), pl6.getTransformedBricks()), true);
	Piece plSample3;
	plSample3 << Brick(0,0,-2,0) << Brick(0,0,-1,0) << Brick(0,0,0,0) << Brick(0,0,1,0); //4-bricks SYMETRICAL l-block on y-axis
	Piece pl7;
	pl7 << Brick(-40,-100,80,90) << Brick(-40,-100,79,90) << Brick(-40,-100,78,90) << Brick(-40,-100,77,90);
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(plSample3.getTransformedBricks(), pl7.getTransformedBricks()), true);
	Piece plSample4;
	plSample4 << Brick(0,0,0,-2) << Brick(0,0,0,-1) << Brick(0,0,0,0) << Brick(0,0,0,1); //4-bricks SYMETRICAL l-block on y-axis
	Piece pl8;
	pl8 << Brick(-40,-100,80,92) << Brick(-40,-100,80,90) << Brick(-40,-100,80,89) << Brick(-40,-100,80,91); //l-block on w-axis with mixed bricks
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(plSample4.getTransformedBricks(), pl8.getTransformedBricks()), true);

	//tesseract 2x2x2x2 (all such tesseracts with any affine transformation are equal when normalized)
	Piece pSmapleTesseract;
	pSmapleTesseract
		<< Brick(-1,0,-1,-1) << Brick(0,0,-1,-1) << Brick(-1,-1,-1,-1) << Brick(0,-1,-1,-1)
		<< Brick(-1,0,0,-1) << Brick(0,0,0,-1) << Brick(-1,-1,0,-1) << Brick(0,-1,0,-1)
		<< Brick(-1,0,-1,0) << Brick(0,0,-1,0) << Brick(-1,-1,-1,0) << Brick(0,-1,-1,0)
		<< Brick(-1,0,0,0) << Brick(0,0,0,0) << Brick(-1,-1,0,0) << Brick(0,-1,0,0)
		;
	Mat<int> testTransformMatrix(5,5);
	testTransformMatrix 
		<< 1 << 0 << 0 << 0 << 666 << endr
		<< 0 << 1 << 0 << 0 << -50 << endr
		<< 0 << 0 << 1 << 0 << 0 << endr
		<< 0 << 0 << 0 << 1 << 2 << endr
		<< 0 << 0 << 0 << 0 << 1 << endr
		;
	Piece pTesseract1(
		PieceHelper::matrixToBricks(
			testTransformMatrix * PieceHelper::bricksToMatrix(pSmapleTesseract.getOriginalBricks())
		)
	);
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pSmapleTesseract.getTransformedBricks(), pTesseract1.getTransformedBricks()), true);
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pSmapleTesseract.getTransformedBricks(), pl8.getTransformedBricks()), false); //just in case...
	testTransformMatrix.clear();
	testTransformMatrix 
		<< 1 << 0 << 0 << 0 << -4 << endr
		<< 0 << 1 << 0 << 0 << -5 << endr
		<< 0 << 0 << 1 << 0 << -6 << endr
		<< 0 << 0 << 0 << 1 << -7 << endr
		<< 0 << 0 << 0 << 0 << 1 << endr
		;
	Piece pTesseract2(
		PieceHelper::matrixToBricks(
			testTransformMatrix * PieceHelper::bricksToMatrix(pSmapleTesseract.getOriginalBricks())
		)
	);
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pSmapleTesseract.getTransformedBricks(), pTesseract2.getTransformedBricks()), true);
	testTransformMatrix.clear();
	testTransformMatrix //rotate XY 90 deg
		<< 0 << 1 << 0 << 0 << 400 << endr
		<< -1 << 0 << 0 << 0 << 0 << endr
		<< 0 << 0 << 1 << 0 << -655 << endr
		<< 0 << 0 << 0 << 1 << 766 << endr
		<< 0 << 0 << 0 << 0 << 1 << endr
		;
	Piece pTesseract3(
		PieceHelper::matrixToBricks(
			testTransformMatrix * PieceHelper::bricksToMatrix(pSmapleTesseract.getOriginalBricks())
		)
	);
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pSmapleTesseract.getTransformedBricks(), pTesseract3.getTransformedBricks()), true);

	//L-block
	Piece pSampleL;
	pSampleL << Brick(-1,1,0,0) << Brick(-1,0,0,0) << Brick(-1,-1,0,0) << Brick(0,-1,0,0);
	Piece pL1;
	pL1 << Brick(-1,1,6,9) << Brick(-1,0,6,9) << Brick(-1,-1,6,9) << Brick(0,-1,6,9);
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pSampleL.getTransformedBricks(), pL1.getTransformedBricks()), true);
}

BOOST_AUTO_TEST_CASE(rotation) {
	//tesseract 1x1x1x1
	Piece pTes1;
	pTes1 << Brick(0,0,0,0);
	Piece pTes2(pTes1);
	//Perform set of rotations
	pTes2.rotateXW();
	pTes2.rotateXY();
	pTes2.rotateXZ();
	pTes2.rotateYW();
	pTes2.rotateYZ();
	pTes2.rotateZW();
	pTes2.rotateYW();
	pTes2.rotateXY();
	pTes2.rotateXZ();
	pTes2.rotateYW();
	pTes2.rotateYZ();
	pTes2.rotateZW();
	pTes2.rotateYW();
	pTes2.rotateXW();
	pTes2.rotateYW();
	pTes2.rotateYW();
	pTes2.rotateXW();
	//must stay the same
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pTes1.getTransformedBricks(), pTes2.getTransformedBricks()), true);
	BOOST_CHECK_EQUAL(pTes1 == pTes2, true); //aslo == operator

	//tesseract 2x2x2x2
	Piece pTes3;
	pTes3
		<< Brick(-1,0,-1,-1) << Brick(0,0,-1,-1) << Brick(-1,-1,-1,-1) << Brick(0,-1,-1,-1)
		<< Brick(-1,0,0,-1) << Brick(0,0,0,-1) << Brick(-1,-1,0,-1) << Brick(0,-1,0,-1)
		<< Brick(-1,0,-1,0) << Brick(0,0,-1,0) << Brick(-1,-1,-1,0) << Brick(0,-1,-1,0)
		<< Brick(-1,0,0,0) << Brick(0,0,0,0) << Brick(-1,-1,0,0) << Brick(0,-1,0,0)
		;
	Piece pTes4(pTes3);
	//Perform set of rotations
	pTes4.rotateXW();
	pTes4.rotateXY();
	pTes4.rotateXZ();
	pTes4.rotateYW();
	pTes4.rotateYZ();
	pTes4.rotateZW();
	pTes4.rotateZW();
	pTes4.rotateYW();
	pTes4.rotateXY();
	pTes4.rotateXZ();
	pTes4.rotateYW();
	pTes4.rotateYZ();
	pTes4.rotateZW();
	pTes4.rotateYW();
	pTes4.rotateXW();
	pTes4.rotateYW();
	pTes4.rotateXW();
	//must stay the same
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pTes3.getTransformedBricks(), pTes4.getTransformedBricks()), true);
	BOOST_CHECK_EQUAL(pTes3 == pTes4, true); //aslo == operator

	//L-block
	//----
	//-x--
	//-x--
	//-xx-
	//----
	Piece pL1;
	pL1 << Brick(-1,1,0,0) << Brick(-1,0,0,0) << Brick(-1,-1,0,0) << Brick(0,-1,0,0);
	//Try to rotate 90deg in the original XY plane (= round the XY plane), withot going to Z or W planes
	Piece pL1Cmp;
	pL1Cmp.removeAllBricks();
	pL1Cmp << Brick(-1,-1,0,0) << Brick(-1,0,0,0) << Brick(0,0,0,0) << Brick(1,0,0,0);
	pL1.rotateXY();
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL1.getTransformedBricks(), pL1Cmp.getTransformedBricks()), true);
	pL1Cmp.removeAllBricks();
	pL1Cmp << Brick(-1,1,0,0) << Brick(0,1,0,0) << Brick(0,0,0,0) << Brick(0,-1,0,0);
	pL1.rotateXY();
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL1.getTransformedBricks(), pL1Cmp.getTransformedBricks()), true);
	pL1Cmp.removeAllBricks();
	pL1Cmp << Brick(0,0,0,0) << Brick(0,-1,0,0) << Brick(-1,-1,0,0) << Brick(-2,-1,0,0);
	pL1.rotateXY();
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL1.getTransformedBricks(), pL1Cmp.getTransformedBricks()), true);
	//4th XY rotation restores the original position
	pL1Cmp.removeAllBricks();
	pL1Cmp << Brick(-1,1,0,0) << Brick(-1,0,0,0) << Brick(-1,-1,0,0) << Brick(0,-1,0,0);
	pL1.rotateXY();
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL1.getTransformedBricks(), pL1Cmp.getTransformedBricks()), true);
	
	//L-block on YZ plane
	Piece pL2;
	pL2 << Brick(0,-1,1,0) << Brick(0,-1,0,0) << Brick(0,-1,-1,0) << Brick(0,0,-1,0);
	Piece pL2Cmp;
	pL2Cmp.removeAllBricks();
	pL2Cmp << Brick(0,-1,-1,0) << Brick(0,-1,0,0) << Brick(0,0,0,0) << Brick(0,1,0,0);
	pL2.rotateYZ();
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL2.getTransformedBricks(), pL2Cmp.getTransformedBricks()), true);
	pL2Cmp.removeAllBricks();
	pL2Cmp << Brick(0,-1,1,0) << Brick(0,0,1,0) << Brick(0,0,0,0) << Brick(0,0,-1,0);
	pL2.rotateYZ();
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL2.getTransformedBricks(), pL2Cmp.getTransformedBricks()), true);
	pL2Cmp.removeAllBricks();
	pL2Cmp << Brick(0,0,0,0) << Brick(0,0,-1,0) << Brick(0,-1,-1,0) << Brick(0,-2,-1,0);
	pL2.rotateYZ();
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL2.getTransformedBricks(), pL2Cmp.getTransformedBricks()), true);
	//4th YZ rotation restores the original position
	pL2Cmp.removeAllBricks();
	pL2Cmp << Brick(0,-1,1,0) << Brick(0,-1,0,0) << Brick(0,-1,-1,0) << Brick(0,0,-1,0);
	pL2.rotateYZ();
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL2.getTransformedBricks(), pL2Cmp.getTransformedBricks()), true);

	//L-block on ZW plane
	Piece pL3;
	pL3 << Brick(0,0,-1,1) << Brick(0,0,-1,0) << Brick(0,0,-1,-1) << Brick(0,0,0,-1);
	Piece pL3Cmp;
	pL3Cmp.removeAllBricks();
	pL3Cmp << Brick(0,0,0,0) << Brick(0,0,0,-1) << Brick(0,0,-1,-1) << Brick(0,0,-2,-1);
	pL3.rotateZW();
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL3.getTransformedBricks(), pL3Cmp.getTransformedBricks()), true);
	pL3Cmp.removeAllBricks();
	pL3Cmp << Brick(0,0,-1,1) << Brick(0,0,0,1) << Brick(0,0,0,0) << Brick(0,0,0,-1);
	pL3.rotateZW();
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL3.getTransformedBricks(), pL3Cmp.getTransformedBricks()), true);
	pL3Cmp.removeAllBricks();
	pL3Cmp << Brick(0,0,-1,-1) << Brick(0,0,-1,0) << Brick(0,0,0,0) << Brick(0,0,1,0);
	pL3.rotateZW();
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL3.getTransformedBricks(), pL3Cmp.getTransformedBricks()), true);
	//4th ZW rotation restores the original position
	pL3Cmp.removeAllBricks();
	pL3Cmp << Brick(0,0,-1,1) << Brick(0,0,-1,0) << Brick(0,0,-1,-1) << Brick(0,0,0,-1);
	pL3.rotateZW();
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL3.getTransformedBricks(), pL3Cmp.getTransformedBricks()), true);
	
	//L-block on XZ plane
	Piece pL4;
	pL4 << Brick(-1,0,1,0) << Brick(-1,0,0,0) << Brick(-1,0,-1,0) << Brick(0,0,-1,0);
	Piece pL4Cmp;
	pL4Cmp.removeAllBricks();
	pL4Cmp << Brick(0,0,0,0) << Brick(0,0,-1,0) << Brick(-1,0,-1,0) << Brick(-2,0,-1,0);
	pL4.rotateXZ();
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL4.getTransformedBricks(), pL4Cmp.getTransformedBricks()), true);
	pL4Cmp.removeAllBricks();
	pL4Cmp << Brick(-1,0,1,0) << Brick(0,0,1,0) << Brick(0,0,0,0) << Brick(0,0,-1,0);
	pL4.rotateXZ();
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL4.getTransformedBricks(), pL4Cmp.getTransformedBricks()), true);
	pL4Cmp.removeAllBricks();
	pL4Cmp << Brick(-1,0,-1,0) << Brick(-1,0,0,0) << Brick(0,0,0,0) << Brick(1,0,0,0);
	pL4.rotateXZ();
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL4.getTransformedBricks(), pL4Cmp.getTransformedBricks()), true);
	//4th XZ rotation restores the original position
	pL4Cmp.removeAllBricks();
	pL4Cmp << Brick(-1,0,1,0) << Brick(-1,0,0,0) << Brick(-1,0,-1,0) << Brick(0,0,-1,0);
	pL4.rotateXZ();
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL4.getTransformedBricks(), pL4Cmp.getTransformedBricks()), true);

	//L-block on XW plane
	Piece pL5;
	pL5 << Brick(-1,0,0,1) << Brick(-1,0,0,0) << Brick(-1,0,0,-1) << Brick(0,0,0,-1);
	Piece pL5Cmp;
	pL5Cmp.removeAllBricks();
	pL5Cmp << Brick(-1,0,0,-1) << Brick(-1,0,0,0) << Brick(0,0,0,0) << Brick(1,0,0,0);
	pL5.rotateXW();
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL5.getTransformedBricks(), pL5Cmp.getTransformedBricks()), true);
	pL5Cmp.removeAllBricks();
	pL5Cmp << Brick(-1,0,0,1) << Brick(0,0,0,1) << Brick(0,0,0,0) << Brick(0,0,0,-1);
	pL5.rotateXW();
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL5.getTransformedBricks(), pL5Cmp.getTransformedBricks()), true);
	pL5Cmp.removeAllBricks();
	pL5Cmp << Brick(0,0,0,0) << Brick(0,0,0,-1) << Brick(-1,0,0,-1) << Brick(-2,0,0,-1);
	pL5.rotateXW();
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL5.getTransformedBricks(), pL5Cmp.getTransformedBricks()), true);
	//4th XW rotation restores the original position
	pL5Cmp.removeAllBricks();
	pL5Cmp << Brick(-1,0,0,1) << Brick(-1,0,0,0) << Brick(-1,0,0,-1) << Brick(0,0,0,-1);
	pL5.rotateXW();
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL5.getTransformedBricks(), pL5Cmp.getTransformedBricks()), true);

	//L-block on YW plane
	Piece pL6;
	pL6 << Brick(0,-1,0,1) << Brick(0,-1,0,0) << Brick(0,-1,0,-1) << Brick(0,0,0,-1);
	Piece pL6Cmp;
	pL6Cmp.removeAllBricks();
	pL6Cmp << Brick(0,0,0,0) << Brick(0,0,0,-1) << Brick(0,-1,0,-1) << Brick(0,-2,0,-1);
	pL6.rotateYW();
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL6.getTransformedBricks(), pL6Cmp.getTransformedBricks()), true);
	pL6Cmp.removeAllBricks();
	pL6Cmp << Brick(0,-1,0,1) << Brick(0,0,0,1) << Brick(0,0,0,0) << Brick(0,0,0,-1);
	pL6.rotateYW();
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL6.getTransformedBricks(), pL6Cmp.getTransformedBricks()), true);
	pL6Cmp.removeAllBricks();
	pL6Cmp << Brick(0,-1,0,-1) << Brick(0,-1,0,0) << Brick(0,0,0,0) << Brick(0,1,0,0);
	pL6.rotateYW();
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL6.getTransformedBricks(), pL6Cmp.getTransformedBricks()), true);
	//4th YW rotation restores the original position
	pL6Cmp.removeAllBricks();
	pL6Cmp << Brick(0,-1,0,1) << Brick(0,-1,0,0) << Brick(0,-1,0,-1) << Brick(0,0,0,-1);
	pL6.rotateYW();
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL6.getTransformedBricks(), pL6Cmp.getTransformedBricks()), true);

	/*//rotate L-block on XY plane, then on YZ plane
	Piece pL7;
	pL7 << Brick(-1,1,0,0) << Brick(-1,0,0,0) << Brick(-1,-1,0,0) << Brick(0,-1,0,0);
	pL7.rotateXY();
	pL7.rotateYZ();
	cout << PieceHelper::bricksToMatrix(pL7.getTransformedBricks());*/
}

BOOST_AUTO_TEST_CASE(shift) {
	//We'll work with an L-block
	//Also need ot work with getOriginalBricks() for cmp piece to get not normalized vector of blocks
	Piece pL1;
	pL1 << Brick(-1,1,0,0) << Brick(-1,0,0,0) << Brick(-1,-1,0,0) << Brick(0,-1,0,0);
	Piece pL1Cmp;
	pL1.moveX(1);
	pL1Cmp.removeAllBricks();
	pL1Cmp << Brick(0,1,0,0) << Brick(0,0,0,0) << Brick(0,-1,0,0) << Brick(1,-1,0,0);
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL1.getTransformedBricks(), pL1Cmp.getOriginalBricks()), true);
	pL1.moveY(1);
	pL1Cmp.removeAllBricks();
	pL1Cmp << Brick(0,2,0,0) << Brick(0,1,0,0) << Brick(0,0,0,0) << Brick(1,0,0,0);
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL1.getTransformedBricks(), pL1Cmp.getOriginalBricks()), true);
	pL1.moveZ(1);
	pL1Cmp.removeAllBricks();
	pL1Cmp << Brick(0,2,1,0) << Brick(0,1,1,0) << Brick(0,0,1,0) << Brick(1,0,1,0);
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL1.getTransformedBricks(), pL1Cmp.getOriginalBricks()), true);
	pL1.moveW(1);
	pL1Cmp.removeAllBricks();
	pL1Cmp << Brick(0,2,1,1) << Brick(0,1,1,1) << Brick(0,0,1,1) << Brick(1,0,1,1);
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL1.getTransformedBricks(), pL1Cmp.getOriginalBricks()), true);
	pL1.moveX(-10);
	pL1Cmp.removeAllBricks();
	pL1Cmp << Brick(-10,2,1,1) << Brick(-10,1,1,1) << Brick(-10,0,1,1) << Brick(-9,0,1,1);
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL1.getTransformedBricks(), pL1Cmp.getOriginalBricks()), true);
}

BOOST_AUTO_TEST_CASE(rotation_and_shift) {
	//We'll work with an L-block
	//Also need ot work with getOriginalBricks() for cmp piece to get not normalized vector of blocks
	Piece pL1Cmp;
	Piece pL1;
	pL1 << Brick(-1,1,0,0) << Brick(-1,0,0,0) << Brick(-1,-1,0,0) << Brick(0,-1,0,0);
	pL1.rotateXY();
	pL1.moveX(1);
	pL1.moveY(1);
	pL1.moveX(1);
	pL1.moveZ(1);
	pL1.moveW(1);
	pL1Cmp.removeAllBricks();
	pL1Cmp << Brick(1,0,1,1) << Brick(1,1,1,1) << Brick(2,1,1,1) << Brick(3,1,1,1);
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL1.getTransformedBricks(), pL1Cmp.getOriginalBricks()), true);
	pL1.rotateXY();
	pL1Cmp.removeAllBricks();
	pL1Cmp << Brick(1,2,1,1) << Brick(2,2,1,1) << Brick(2,1,1,1) << Brick(2,0,1,1);
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL1.getTransformedBricks(), pL1Cmp.getOriginalBricks()), true);
}

BOOST_AUTO_TEST_CASE(save_load) {
	Piece pL1;
	pL1 << Brick(-1,1,0,0) << Brick(-1,0,0,0) << Brick(-1,-1,0,0) << Brick(0,-1,0,0);
	pL1.setDifficultyLevel(1);
	pL1.move(3,40,500,6000);
	pL1.rotate(90,180,270,360,0,90);
	pL1.saveToFile("testLblock1.piece");
	//try to load this file and recreate pL1 piece
	Piece pL1Loaded(string("testLblock1.piece"));
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL1.getOriginalBricks(), pL1Loaded.getOriginalBricks()), true);
	BOOST_CHECK_EQUAL(PieceHelper::comapreBricksVectors(pL1.getTransformedBricks(), pL1Loaded.getTransformedBricks()), true);
	BOOST_CHECK_EQUAL(pL1.saveToFile(":::"), false); //can't write to file
	remove("testLblock1.piece"); //delte file

	ofstream myfile;
	myfile.open ("testBad.piece");
	myfile << "<?xml?>\n<piece />";
	myfile.close();
	BOOST_CHECK_THROW(Piece(string("testBad.piece")), PieceExceptions::WrongFileStructure);
	remove("testBad.piece"); //delte file
}

BOOST_AUTO_TEST_SUITE_END( )