#include "Cup3dProjection.h"

map<string, umat> Cup3dProjection::_emptiesCache; 

Cup3dProjection::Cup3dProjection(void) {
}

Cup3dProjection::Cup3dProjection(unsigned int sizeX, unsigned int sizeY, unsigned int sizeZ) {
	_grid = Cube<gridCell>(sizeX, sizeY, sizeZ);
	_grid.fill(GRIDCELL_NOTHING); //by defaul there is nothing in our projection
}

Cup3dProjection::~Cup3dProjection(void) {
}

unsigned int Cup3dProjection::getSizeX() const {
	return _grid.GRID_SIZE_X;
}

unsigned int Cup3dProjection::getSizeY() const {
	return _grid.GRID_SIZE_Y;
}

unsigned int Cup3dProjection::getSizeZ() const {
	return _grid.GRID_SIZE_Z;
}

const Cube<gridCell>& Cup3dProjection::getGrid() const {
	return _grid;
}

inline gridCell Cup3dProjection::at(unsigned int x, unsigned int y, unsigned int z) const {
	return _grid.at(x, y, z);
}