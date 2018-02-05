#pragma once
#include <memory>
#include <atomic>
#include <armadillo>

#include "GameObject.h"
#include "TransformationMatrices.h"

using namespace std;
using namespace arma;

class Camera : public GameObject {
public:
	struct Angles {
	private:
		void normalize() {
			if (degreeX >= 360.0f) degreeX -= 360.0f;
			if (degreeX <= -360.0f) degreeX += 360.0f;
			if (degreeY >= 360.0f) degreeY -= 360.0f;
			if (degreeY <= -360.0f) degreeY += 360.0f;
			if (degreeZ >= 360.0f) degreeZ -= 360.0f;
			if (degreeZ <= -360.0f) degreeZ += 360.0f;
		}
	public:
		Angles(float _degreeX, float _degreeY, float _degreeZ) : degreeX(_degreeX), degreeY(_degreeY), degreeZ(_degreeZ) {}
		Angles() : degreeX(0.0f), degreeY(0.0f), degreeZ(0.0f) {}
		float degreeX;
		float degreeY;
		float degreeZ;
		inline Angles& operator+=(const Angles& right) {
			degreeX += right.degreeX;
			degreeY += right.degreeY;
			degreeZ += right.degreeZ;
			normalize();
			return *this;
		};
		inline Angles operator+(const Angles& right) const {
			Angles ret(degreeX + right.degreeX, degreeY + right.degreeY, degreeZ + right.degreeZ);
			ret.normalize();
			return ret;
		};
		inline Angles& operator-=(const Angles& right) {
			degreeX -= right.degreeX;
			degreeY -= right.degreeY;
			degreeZ -= right.degreeZ;
			normalize();
			return *this;
		};
		inline Angles operator-(const Angles& right) const {
			Angles ret(degreeX - right.degreeX, degreeY - right.degreeY, degreeZ - right.degreeZ);
			ret.normalize();
			return ret;
		};
		inline bool operator==(const Angles& right) const {
			return (degreeX == right.degreeX && degreeY == right.degreeY && degreeZ == right.degreeZ);
		};
	};
	struct AnglesLimit {
		AnglesLimit(Angles _min, Angles _max) : minimum(_min), maximum(_max) {}
		AnglesLimit() : minimum(-360.0f, -360.0f, -360.0f), maximum(360.0f, 360.0f, 360.0f) {}
		Angles minimum;
		Angles maximum;
		inline bool test(const Angles& a) const {
			return
				(a.degreeX >= minimum.degreeX && a.degreeX <= maximum.degreeX
				&&
				a.degreeY >= minimum.degreeY && a.degreeY <= maximum.degreeY
				&&
				a.degreeZ >= minimum.degreeZ && a.degreeZ <= maximum.degreeZ);
		};
	};
	struct Coordinates {
		Coordinates(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
		Coordinates() : x(0.0f), y(0.0f), z(0.0f) {}
		float x;
		float y;
		float z;
		inline Coordinates& operator+=(const Coordinates& right) {
			x += right.x;
			y += right.y;
			z += right.z;
			return *this;
		};
		inline Coordinates operator+(const Coordinates& right) const {
			return Coordinates(x + right.x, y + right.y, z + right.z);
		};
		inline Coordinates& operator-=(const Coordinates& right) {
			x -= right.x;
			y -= right.y;
			z -= right.z;
			return *this;
		};
		inline Coordinates operator-(const Coordinates& right) const {
			return Coordinates(x - right.x, y - right.y, z - right.z);
		};
		inline bool operator==(const Coordinates& right) const {
			return (x == right.x && y == right.y && z == right.z);
		};
	};
	struct CoordinatesLimit {
		CoordinatesLimit(Coordinates _min, Coordinates _max) : minimum(_min), maximum(_max) {}
		CoordinatesLimit() : minimum(-1000.0f, -1000.0f, -1000.0f), maximum(1000.0f, 1000.0f, 1000.0f) {}
		Coordinates minimum;
		Coordinates maximum;
		inline bool test(const Coordinates& c) const {
			return
				(c.x >= minimum.x && c.x <= maximum.x
				&&
				c.y >= minimum.y && c.y <= maximum.y
				&&
				c.z >= minimum.z && c.z <= maximum.z);
		}
	};
	struct ZoomLimit {
		ZoomLimit(float _min, float _max) : minimum(_min), maximum(_max) {}
		ZoomLimit() : minimum(0.01f), maximum(100.0f) {}
		float minimum;
		float maximum;
		inline bool test(float z) const {
			return (z >= minimum && z <= maximum);
		}
	};
private:
	fmat44 _defaultHeadRotationMatrix;
	Angles _defaultHeadRotationAngles;
	fmat44 _defaultPositionMatrix;
	fmat44 _defaultCirclingAroundMatrix;
	Angles _defaultCirclingAroundAngles;
	fmat44 _defaultCirclingDistanceMatrix;
	float _defaultZoom;
	
	fmat44 _headRotationMatrix;
	Angles _headRotationAngles;
	AnglesLimit _headRotationLimits;

	fmat44 _positionMatrix;
	CoordinatesLimit _positionLimits;

	fmat44 _circlingAroundMatrix;
	Angles _circlingAroundAngles;
	AnglesLimit _circlingAroundLimits;
	Coordinates _circlingAroundBasis;
	CoordinatesLimit _circlingAroundBasisLimits;

	fmat44 _circlingDistanceMatrix;
	CoordinatesLimit _circlingDistanceLimits;

	float _zoom;
	ZoomLimit _zoomLimits;

protected:
	virtual void init();
public:
	Camera(void);

	virtual void positionSetDefault(const Coordinates& coordinates = Coordinates());
	virtual void positionReset();
	virtual void positionSet(const Coordinates& coordinates);
	virtual void positionMove(const Coordinates& coordinates);
	virtual CoordinatesLimit positionLimitsGet() const;
	virtual bool positionLimitsTest(const Coordinates& coordinates) const;
	virtual void positionLimitsSet(const CoordinatesLimit& coordinatesLimit);

	virtual void headRotationSetDefault(const Angles& degrees = Angles(), RotationOrder order = RotationOrder::xyz);
	virtual void headRotationReset();
	virtual void headRotationSet(const Angles& degrees, RotationOrder order = RotationOrder::xyz);
	virtual void headRotationRotate(const Angles& degrees, RotationOrder order = RotationOrder::xyz);
	virtual AnglesLimit headRotationLimitsGet() const;
	virtual bool headRotationLimitsTest(const Angles& degrees) const;
	virtual void headRotationLimitsSet(const AnglesLimit& degreesLimit);

	virtual void circlingDistanceSetDefault(const Coordinates& coordinates = Coordinates());
	virtual void circlingDistanceReset();
	virtual void circlingDistanceSet(const Coordinates& coordinates);
	virtual void circlingDistanceMove(const Coordinates& coordinates);
	virtual CoordinatesLimit circlingDistanceLimitsGet() const;
	virtual bool circlingDistanceLimitsTest(const Coordinates& coordinates) const;
	virtual void circlingDistanceLimitsSet(const CoordinatesLimit& coordinatesLimit);

	virtual void circlingAroundSetDefault(const Angles& degrees = Angles(), RotationOrder order = RotationOrder::xyz);
	virtual void circlingAroundReset();
	virtual void circlingAroundSet(const Angles& degrees, RotationOrder order = RotationOrder::xyz);
	virtual void circlingAroundCircle(const Angles& degrees, RotationOrder order = RotationOrder::xyz);
	virtual void circlingAroundMoveBasis(const Coordinates& coordinates); //TODO: also add basis limits and checks
	virtual AnglesLimit circlingAroundLimitsGet() const;
	virtual bool circlingAroundLimitsTest(const Angles& degrees) const;
	virtual void circlingAroundLimitsSet(const AnglesLimit& degreesLimit);
	virtual CoordinatesLimit circlingAroundBasisLimitsGet() const;
	virtual bool circlingAroundBasisLimitsTest(const Coordinates& degrees) const;
	virtual void circlingAroundBasisLimitsSet(const CoordinatesLimit& degreesLimit);

	virtual void zoomSetDefault(float zoom = 1.0f);
	virtual void zoomReset();
	virtual void zoomSet(float zoom);
	virtual void zoom(float zoom);
	virtual void zoomMult(float zoom);
	virtual void zoomAdd(float zoom);
	virtual ZoomLimit zoomLimitsGet() const;
	virtual bool zoomLimitsTest(float zoom) const;
	virtual void zoomLimitsSet(ZoomLimit zoomLimit);

	virtual void projectionReset() = 0;

	virtual void resetAll() final;


	virtual Angles getHeadRotation() const;
	virtual fmat44 getHeadRotationMatrix() const;

	virtual Coordinates getPosition() const;
	virtual fmat44 getPositionMatrix() const;

	virtual Angles getCirclingAround() const;
	virtual fmat44 getCirclingArounMatrix() const;

	virtual Coordinates getCirclingDistance() const;
	virtual fmat44 getCirclingDistanceMatrix() const;

	virtual fmat44 getViewMatrix() const;

	virtual float getZoom() const;

	virtual fmat44 getProjectionMatrix() const = 0;
	
	virtual fmat44 getViewProjectionMatrix() const final;

	//Beware! It will reset any previous parameters. headRotation and position limits will be checked.
	virtual void lookAt(const Coordinates& lookFrom = Coordinates(0.0f, 0.0f, 0.0f), const Coordinates& lookAt = Coordinates(0.0f, 0.0f, -1.0f), float rotZdeg = 0.0f);
};
typedef shared_ptr<Camera> CameraPtr;


class CameraPerspective : public Camera {
public:
	struct Projection {
		Projection(float _fovX, float _fovY, float _zNear, float _zFar) : fovX(_fovX), fovY(_fovY), zNear(_zNear), zFar(_zFar) {}
		Projection() : fovX(90.0f), fovY(66.6f), zNear(1.0f), zFar(100.0f) {}
		float fovX;
		float fovY;
		float zNear;
		float zFar;
		inline bool operator==(const Projection& right) const {
			return (fovX == right.fovX && fovY == right.fovY && zNear == right.zNear && zFar == right.zFar);
		};
	};
private:
	Projection _defaultProjection;
	Projection _projection;

protected:
	virtual void init();
public:
	virtual void projectionSetDefault(const Projection& projection = Projection());
	virtual void projectionSet(const Projection& projection);
	virtual void projectionReset();
	virtual fmat44 getProjectionMatrix() const;
};

/*class CameraOrthographic : public Camera {
};*/

//TODO: camera director