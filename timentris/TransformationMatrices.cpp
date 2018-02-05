#include "TransformationMatrices.h"

fmat44 TransformationMatrices::getTranslationMatrix(float x, float y, float z) {
	fmat44 m(fill::eye);
	m(0,3) = x;
	m(1,3) = y;
	m(2,3) = z;
	return m;
}

fmat44 TransformationMatrices::getScaleMatrix(float scale) {
	fmat44 m(fill::eye);
	m(0,0) = scale;
	m(1,1) = scale;
	m(2,2) = scale;
	return m;
}

fmat44 TransformationMatrices::getScaleMatrix(float scaleX, float scaleY, float scaleZ) {
	fmat44 m(fill::eye);
	m(0,0) = scaleX;
	m(1,1) = scaleY;
	m(2,2) = scaleZ;
	return m;
}

fmat44 TransformationMatrices::getScaleXMatrix(float scale) {
	return getScaleMatrix(scale, 0, 0);
}

fmat44 TransformationMatrices::getScaleYMatrix(float scale) {
	return getScaleMatrix(0, scale, 0);
}

fmat44 TransformationMatrices::getScaleZMatrix(float scale) {
	return getScaleMatrix(0, 0, scale);
}

fmat44 TransformationMatrices::getRotationXMatrix(float degree) {
	//to radians
	degree = degree * fdatum::pi / 180;
	fmat44 m(fill::eye);
	m(1,1) = cosf(degree); m(1,2) = -sinf(degree);
	m(2,1) = sinf(degree); m(2,2) = cosf(degree);
	return m;
}

fmat44 TransformationMatrices::getRotationYMatrix(float degree) {
	//to radians
	degree = degree * fdatum::pi / 180;
	fmat44 m(fill::eye);
	m(0,0) = cosf(degree);  m(0,2) = sinf(degree);
	m(2,0) = -sinf(degree); m(2,2) = cosf(degree);
	return m;
}

fmat44 TransformationMatrices::getRotationZMatrix(float degree) {
	//to radians
	degree = degree * fdatum::pi / 180;
	fmat44 m(fill::eye);
	m(0,0) = cosf(degree); m(0,1) = -sinf(degree);
	m(1,0) = sinf(degree); m(1,1) = cosf(degree);
	return m;
}

fmat44 TransformationMatrices::getRotationMatrix(float degreeX, float degreeY, float degreeZ, RotationOrder order) {
	/*//to radians
	degreeX = degreeX * fdatum::pi / 180;
	degreeY = degreeY * fdatum::pi / 180;
	degreeZ = degreeZ * fdatum::pi / 180;*/
	fmat44 m(fill::eye);
	switch(order) {
	case RotationOrder::xyz:
		return fmat44(getRotationZMatrix(degreeZ) * getRotationYMatrix(degreeY) * getRotationXMatrix(degreeX));
		break;
	case RotationOrder::xzy:
		return fmat44(getRotationYMatrix(degreeY) * getRotationZMatrix(degreeZ) * getRotationXMatrix(degreeX));
		break;
	case RotationOrder::yxz:
		return fmat44(getRotationZMatrix(degreeZ) * getRotationXMatrix(degreeX) * getRotationYMatrix(degreeY));
		break;
	case RotationOrder::yzx:
		return fmat44(getRotationXMatrix(degreeX) * getRotationZMatrix(degreeZ) * getRotationYMatrix(degreeY));
		break;
	case RotationOrder::zxy:
		return fmat44(getRotationYMatrix(degreeY) * getRotationXMatrix(degreeX) * getRotationZMatrix(degreeZ));
		break;
	case RotationOrder::zyx:
		return fmat44(getRotationXMatrix(degreeX) * getRotationYMatrix(degreeY) * getRotationZMatrix(degreeZ));
		break;
	default:
		return fmat44(fill::eye);
		break;
	}
	return fmat44(fill::eye);
}
