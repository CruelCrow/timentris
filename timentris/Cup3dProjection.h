#pragma once
#include <memory>
#include <vector>
#include <map>
#include <boost/date_time.hpp>
#include <boost/timer.hpp>
#include "Brick.h"
#include "Piece.h"
#include "exceptions.h"

#define SUBSPACE_XYZ 0
#define SUBSPACE_XYW 1
#define SUBSPACE_XZW 2
#define SUBSPACE_YZW 3

typedef unsigned int gridCell; //single cell in Cup<int> 3d grid

#define GRIDCELL_NOTHING 0
#define GRIDCELL_PIECE_BRICK 1
#define GRIDCELL_CUP_BRICK 2

#define GRID_SIZE_X n_rows
#define GRID_SIZE_Y n_cols
#define GRID_SIZE_Z n_slices

//informative-only projection of a 4D CUp to our 3D world
class Cup3dProjection
{
	friend class Cup; //Cup has a full control over it's projections
protected:
	static map<string, umat> _emptiesCache; 

	Cube<gridCell> _grid; //3d grid
public:
	Cup3dProjection(void); //default constructor for an array allocation
	Cup3dProjection(unsigned int sizeX, unsigned int sizeY, unsigned int sizeZ);
	~Cup3dProjection(void);

	unsigned int getSizeX() const;
	unsigned int getSizeY() const;
	unsigned int getSizeZ() const;

	const Cube<gridCell>& getGrid() const;
	inline gridCell at(unsigned int x, unsigned int y, unsigned int z) const; //what is palced on that coordinate
};

class Cup3dSlice : public Cup3dProjection {
public:
	Cup3dSlice(void) : Cup3dProjection(){}
	Cup3dSlice(unsigned int sizeX, unsigned int sizeY, unsigned int sizeZ) : Cup3dProjection(sizeX, sizeY, sizeZ) {}
};

class Cup3dSubspace : public Cup3dProjection {
public:
	Cup3dSubspace(void) : Cup3dProjection(){}
	Cup3dSubspace(unsigned int sizeX, unsigned int sizeY, unsigned int sizeZ) : Cup3dProjection(sizeX, sizeY, sizeZ) {}
};