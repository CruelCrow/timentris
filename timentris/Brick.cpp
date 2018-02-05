#include "Brick.h"

Brick::Brick() {
	setX(0);
	setY(0);
	setZ(0);
	setW(0);
}

Brick::Brick(const int x=0, const int y=0, const int z=0, const int w=0) {
	setX(x);
	setY(y);
	setZ(z);
	setW(w);
}

Brick::Brick(const Mat<int>& singleColumn) {
	*this = singleColumn; //cal matrix assigment operator
}

Brick::~Brick(void) {
}

void Brick::setX(const int x) {
	_x = x;
}
void Brick::setY(const int y) {
	_y = y;
}
void Brick::setZ(const int z) {
	_z = z;
}
void Brick::setW(const int w) {
	_w = w;
}

int Brick::getX() const {
	return _x;
}
int Brick::getY() const {
	return _y;
}
int Brick::getZ() const {
	return _z;
}
int Brick::getW() const {
	return _w;
}

bool Brick::operator== (const Brick &b) const {
	return (
		getX() == b.getX()
		&&
		getY() == b.getY()
		&&
		getZ() == b.getZ()
		&&
		getW() == b.getW()
		);
}
 
bool Brick::operator!= (const Brick &b) const {
    return !(*this == b);
}

Brick::operator Mat<int>() const {
	Mat<int> brickRepresantation(5,1);
	brickRepresantation << getX() << endr
						<< getY() << endr
						<< getZ() << endr
						<< getW() << endr
						<< 1 << endr
						;
	return brickRepresantation;
}

Brick& Brick::operator=(const Mat<int>& singleColumn) {
	if (singleColumn.n_cols != 1 || (singleColumn.n_rows != 4 && singleColumn.n_rows != 5)) {
		throw BrickExceptions::WrongMatrixSize();
	}
	setX(singleColumn(0,0));
	setY(singleColumn(1,0));
	setZ(singleColumn(2,0));
	setW(singleColumn(3,0));
    return *this;
}

int& Brick::operator[](char element) {
	switch (element) {
	case 'x':
		return _x;
		break;
	case 'y':
		return _y;
		break;
	case 'z':
		return _z;
		break;
	case 'w':
		return _w;
		break;
	default:
		throw BrickExceptions::WrongCoordianteAlias();
	}
}

const int Brick::operator[](char element) const {
	switch (element) {
	case 'x':
		return getX();
		break;
	case 'y':
		return getY();
		break;
	case 'z':
		return getZ();
		break;
	case 'w':
		return getW();
		break;
	default:
		throw BrickExceptions::WrongCoordianteAlias();
	}
}