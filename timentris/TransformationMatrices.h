#pragma once
#include <armadillo>
using namespace std;
using namespace arma;

enum class RotationOrder {
	xyz,
	xzy,
	yxz,
	yzx,
	zxy,
	zyx
};

class TransformationMatrices {
public:
	static fmat44 getTranslationMatrix(float x, float y, float z);
	static fmat44 getScaleMatrix(float scale);
	static fmat44 getScaleMatrix(float scaleX, float scaleY, float scaleZ);
	static fmat44 getScaleXMatrix(float scale);
	static fmat44 getScaleYMatrix(float scale);
	static fmat44 getScaleZMatrix(float scale);
	static fmat44 getRotationXMatrix(float degree);
	static fmat44 getRotationYMatrix(float degree);
	static fmat44 getRotationZMatrix(float degree);
	static fmat44 getRotationMatrix(float degreeX, float degreeY, float degreeZ, RotationOrder order = RotationOrder::xyz);
};
typedef TransformationMatrices Matrices;

/// <image url="$(ProjectDir)\comments\gl_anglestoaxes01.png" scale="0.75" />