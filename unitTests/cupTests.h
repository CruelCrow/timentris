#include "unitTests.h"
#include "Cup.h"

BOOST_AUTO_TEST_SUITE(cup_test)

BOOST_AUTO_TEST_CASE(constructors) {
	Cup c1 = Cup(10,11,15,12);
	BOOST_CHECK_EQUAL(c1.getSizeX(), 10);
	BOOST_CHECK_EQUAL(c1.getSizeY(), 11);
	BOOST_CHECK_EQUAL(c1.getSizeZ(), 15);
	BOOST_CHECK_EQUAL(c1.getSizeW(), 12);
	c1 << Brick(0,0,0,0) << Brick(1,0,0,0) << Brick(0,1,0,0) << Brick(0,0,1,0) << Brick(0,0,0,1);
	Piece p1;
	p1 << Brick(-2,0,0,0) << Brick(-1,0,0,0) << Brick(0,0,0,0) << Brick(1,0,0,0);
	c1 << p1;
	
	BOOST_CHECK_THROW(Cup(0,0,0,0), CupExceptions::WrongCupSize);
	BOOST_CHECK_THROW(Cup(0,11,15,12), CupExceptions::WrongCupSize);
	BOOST_CHECK_THROW(Cup(0,0,15,12), CupExceptions::WrongCupSize);
	BOOST_CHECK_THROW(Cup(0,11,0,12), CupExceptions::WrongCupSize);
	BOOST_CHECK_THROW(Cup(0,11,15,0), CupExceptions::WrongCupSize);

	//check copy constructor
	Cup c2(c1);
	BOOST_CHECK_EQUAL(&c2 == &c1, false); //different addresses
	//BOOST_CHECK_EQUAL(c2.getPiecePtr() != c1.getPiecePtr(), true); //compare addresses of pieces
	BOOST_CHECK_EQUAL(c2.getPiece() == c1.getPiece(), true); //comapre pieces themselves
	BOOST_CHECK_EQUAL(c2.getSizeX(), 10);
	BOOST_CHECK_EQUAL(c2.getSizeY(), 11);
	BOOST_CHECK_EQUAL(c2.getSizeZ(), 15);
	BOOST_CHECK_EQUAL(c2.getSizeW(), 12);
	BOOST_CHECK_EQUAL(c2.isBrickExists(0,0,0,0), true);
	BOOST_CHECK_EQUAL(c2.isBrickExists(1,0,0,0), true);
	BOOST_CHECK_EQUAL(c2.isBrickExists(0,1,0,0), true);
	BOOST_CHECK_EQUAL(c2.isBrickExists(0,0,1,0), true);
	BOOST_CHECK_EQUAL(c2.isBrickExists(0,0,0,1), true);
	BOOST_CHECK_EQUAL(c2.isPieceExists(), true);
	c2.removePiece();
	BOOST_CHECK_EQUAL(c2.isPieceExists(), false);
	BOOST_CHECK_EQUAL(c1.isPieceExists(), true);
	c2.demolishBrick(0,0,0,0);
	BOOST_CHECK_EQUAL(c2.getNumberOfDemolishedBricks(), 1);
	BOOST_CHECK_EQUAL(c1.getNumberOfDemolishedBricks(), 0);

	Cup c3(1,1,1,1);
	c3 = c1; //operator=
	BOOST_CHECK_EQUAL(&c3 == &c1, false); //different addresses
	BOOST_CHECK_EQUAL(&c3 == &c2, false); //different addresses
	//BOOST_CHECK_EQUAL(c3.getPiecePtr() != c1.getPiecePtr(), true); //compare addresses of pieces
	BOOST_CHECK_EQUAL(c3.getPiece() == c1.getPiece(), true); //comapre pieces themselves
	BOOST_CHECK_EQUAL(c3.getSizeX(), 10);
	BOOST_CHECK_EQUAL(c3.getSizeY(), 11);
	BOOST_CHECK_EQUAL(c3.getSizeZ(), 15);
	BOOST_CHECK_EQUAL(c3.getSizeW(), 12);
	BOOST_CHECK_EQUAL(c3.isBrickExists(0,0,0,0), true);
	BOOST_CHECK_EQUAL(c3.isBrickExists(1,0,0,0), true);
	BOOST_CHECK_EQUAL(c3.isBrickExists(0,1,0,0), true);
	BOOST_CHECK_EQUAL(c3.isBrickExists(0,0,1,0), true);
	BOOST_CHECK_EQUAL(c3.isBrickExists(0,0,0,1), true);
	BOOST_CHECK_EQUAL(c3.isPieceExists(), true);
	c3.removePiece();
	BOOST_CHECK_EQUAL(c3.isPieceExists(), false);
	BOOST_CHECK_EQUAL(c1.isPieceExists(), true);
	c3.demolishBrick(0,0,0,0);
	BOOST_CHECK_EQUAL(c1.getNumberOfDemolishedBricks(), 0);

	Cup c4(2,4,6,8);
	c4 = c3; //operator=
	BOOST_CHECK_EQUAL(&c4 == &c3, false); //different addresses
	BOOST_CHECK_EQUAL(c4.isBrickExists(1,0,0,0), true);
	BOOST_CHECK_EQUAL(c4.isBrickExists(0,1,0,0), true);
	BOOST_CHECK_EQUAL(c4.isBrickExists(0,0,1,0), true);
	BOOST_CHECK_EQUAL(c4.isBrickExists(0,0,0,1), true);
	BOOST_CHECK_EQUAL(c4.getNumberOfDemolishedBricks(), 1);
	BOOST_CHECK_EQUAL(c4.isPieceExists(), false);
}

BOOST_AUTO_TEST_CASE(bricks) {
	Cup c1 = Cup(10,10,15,10);
	c1 << Brick(0,0,0,0) << Brick(1,0,0,0) << Brick(0,1,0,0) << Brick(0,0,1,0) << Brick(0,0,0,1);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,0,0,0), true);
	BOOST_CHECK_EQUAL(c1.isBrickExists(1,0,0,0), true);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,1,0,0), true);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,0,1,0), true);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,0,0,1), true);
	BOOST_CHECK_EQUAL(c1.isBrickExists(1,0,0,1), false);
	BOOST_CHECK_EQUAL(c1.getNumberOfBricks(), 5);
	c1.removeBrick(0,0,0,0);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,0,0,0), false);
	BOOST_CHECK_EQUAL(c1.isBrickExists(1,0,0,0), true);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,1,0,0), true);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,0,1,0), true);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,0,0,1), true);
	BOOST_CHECK_EQUAL(c1.isBrickExists(1,0,0,1), false);
	BOOST_CHECK_THROW(c1.removeBrick(0,0,0,0), CupExceptions::BrickNotFound);
	c1.addBrick(2,2,2,2);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,0,0,0), false);
	BOOST_CHECK_EQUAL(c1.isBrickExists(1,0,0,0), true);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,1,0,0), true);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,0,1,0), true);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,0,0,1), true);
	BOOST_CHECK_EQUAL(c1.isBrickExists(1,0,0,1), false);
	BOOST_CHECK_EQUAL(c1.isBrickExists(2,2,2,2), true);
	BOOST_CHECK_THROW(c1.addBrick(2,2,2,2), CupExceptions::BrickAlreadyExists);
	BOOST_CHECK_THROW(c1 << Brick(1,0,0,0), CupExceptions::BrickAlreadyExists);
	c1.removeAllBricks();
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,0,0,0), false);
	BOOST_CHECK_EQUAL(c1.isBrickExists(1,0,0,0), false);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,1,0,0), false);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,0,1,0), false);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,0,0,1), false);
	BOOST_CHECK_EQUAL(c1.isBrickExists(1,0,0,1), false);
	BOOST_CHECK_EQUAL(c1.isBrickExists(2,2,2,2), false);
	c1 << Brick(0,0,0,0) << Brick(1,0,0,0) << Brick(0,1,0,0) << Brick(0,0,1,0) << Brick(0,0,0,1);
	c1.demolishBrick(0,0,0,0);
	BOOST_CHECK_EQUAL(c1.getNumberOfDemolishedBricks(), 1);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,0,0,0), false);
	BOOST_CHECK_EQUAL(c1.isBrickExists(1,0,0,0), true);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,1,0,0), true);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,0,1,0), true);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,0,0,1), true);
	BOOST_CHECK_EQUAL(c1.isBrickExists(1,0,0,1), false);
	BOOST_CHECK_THROW(c1.demolishBrick(0,0,0,0), CupExceptions::BrickNotFound);
	c1.demolishAllBricks();
	BOOST_CHECK_EQUAL(c1.getNumberOfDemolishedBricks(), 5);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,0,0,0), false);
	BOOST_CHECK_EQUAL(c1.isBrickExists(1,0,0,0), false);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,1,0,0), false);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,0,1,0), false);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,0,0,1), false);
	BOOST_CHECK_EQUAL(c1.isBrickExists(1,0,0,1), false);
	//try to add brick outside the Cup
	BOOST_CHECK_THROW(c1 << Brick(10,0,0,0), CupExceptions::WrongBrickPosition);
	BOOST_CHECK_THROW(c1 << Brick(0,11,0,0), CupExceptions::WrongBrickPosition);
	BOOST_CHECK_THROW(c1 << Brick(0,0,15,0), CupExceptions::WrongBrickPosition);
	BOOST_CHECK_THROW(c1 << Brick(0,0,0,12), CupExceptions::WrongBrickPosition);

	c1 << Brick(0,0,0,0) << Brick(1,0,0,0) << Brick(2,0,0,0) << Brick(3,0,0,0) << Brick(4,0,0,0) << Brick(5,0,0,0) << Brick(6,0,0,0);
	c1 << Brick(0,1,0,0) << Brick(0,6,0,0) << Brick(0,8,0,0) << Brick(0,9,0,0);
	c1 << Brick(0,0,0,5) << Brick(0,0,0,7);
	c1 << Brick(0,0,1,5) << Brick(0,0,1,7); //two bricks on the second line
	BOOST_CHECK_EQUAL(c1.getNumberOfBricks(), 15);
	c1.demolishLine(0);
	BOOST_CHECK_EQUAL(c1.getNumberOfBricks(), 2);
	BOOST_CHECK_EQUAL(c1.getNumberOfDemolishedBricks(), 5 + 13);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,0,0,0), false);
	BOOST_CHECK_EQUAL(c1.isBrickExists(1,0,0,0), false);
	BOOST_CHECK_EQUAL(c1.isBrickExists(2,0,0,0), false);
	BOOST_CHECK_EQUAL(c1.isBrickExists(3,0,0,0), false);
	BOOST_CHECK_EQUAL(c1.isBrickExists(4,0,0,0), false);
	BOOST_CHECK_EQUAL(c1.isBrickExists(5,0,0,0), false);
	BOOST_CHECK_EQUAL(c1.isBrickExists(6,0,0,0), false);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,1,0,0), false);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,6,0,0), false);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,8,0,0), false);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,9,0,0), false);
	//those two bricks will exist, because they descended from the second line
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,0,0,5), true);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,0,0,7), true);
	//bricks on the second line is not existing anymore, because they descend to the first line
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,0,1,5), false);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,0,1,7), false);

	c1 << Brick(0,0,1,5) << Brick(0,0,1,7); //place some bricks on second line
	c1 << Brick(0,0,2,5) << Brick(0,0,2,7); //place some bricks on third line
	vector<unsigned int> linesToDemolish;
	linesToDemolish.push_back(0);
	linesToDemolish.push_back(1);
	c1.demolishLines(linesToDemolish);
	BOOST_CHECK_EQUAL(c1.getNumberOfBricks(), 2);
	BOOST_CHECK_EQUAL(c1.getNumberOfDemolishedBricks(), 5 + 13 + 4);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,0,1,5), false);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,0,1,7), false);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,0,2,5), false);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,0,2,7), false);
	//two pieces descend from the third line to the first line:
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,0,0,5), true);
	BOOST_CHECK_EQUAL(c1.isBrickExists(0,0,0,7), true);
}

BOOST_AUTO_TEST_CASE(pieces) {
}

BOOST_AUTO_TEST_SUITE_END( )