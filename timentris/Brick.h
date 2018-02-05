#pragma once
#include <armadillo>
#include "exceptions.h"
#include "GameObject.h"

using namespace std;
using namespace arma;

class Brick : public GameObject {
private:
	int _x;
	int _y;
	int _z;
	int _w;
public:
	Brick();
	Brick(const int x, const int y, const int z, const int w);
	Brick(const Mat<int>& singleColumn);
	~Brick(void);

	void setX(const int x);
	void setY(const int y);
	void setZ(const int z);
	void setW(const int w);
	int getX() const;
	int getY() const;
	int getZ() const;
	int getW() const;
	
	bool operator== (const Brick &b) const;
	bool operator!= (const Brick &b) const;
	operator Mat<int>() const; //allow cast to single-column matrix
	Brick& operator= (const Mat<int>& singleColumn); //allow assigment from single-column matrix (with 4 or 5 rows, when 5th row is not important)
	int& operator[](char element); //set
	const int operator[](char element) const; //get
};