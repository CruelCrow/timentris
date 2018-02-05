#include "Camera.h"


Camera::Camera(void) {
	init();
}

void Camera::init() {
	positionSetDefault();
	positionReset();
	headRotationSetDefault();
	headRotationReset();
	circlingDistanceSetDefault();
	circlingDistanceReset();
	circlingAroundSetDefault();
	circlingAroundReset();
	zoomSetDefault();
	zoomReset();
}


void Camera::positionSetDefault(const Coordinates& coordinates) {
	_defaultPositionMatrix = TransformationMatrices::getTranslationMatrix(coordinates.x, coordinates.y, coordinates.z);
}

void Camera::positionReset() {
	_positionMatrix = _defaultPositionMatrix;
}

void Camera::positionSet(const Coordinates& coordinates) {
	if (positionLimitsTest(coordinates))
		_positionMatrix = TransformationMatrices::getTranslationMatrix(-coordinates.x, -coordinates.y, coordinates.z);
}

void Camera::positionMove(const Coordinates& coordinates) {
	Coordinates newCoordiantes(_positionMatrix.at(0,3) - coordinates.x, _positionMatrix.at(1,3) - coordinates.y, _positionMatrix.at(2,3) + coordinates.z);
	if (positionLimitsTest(newCoordiantes)) {
		_positionMatrix.at(0,3) = newCoordiantes.x;
		_positionMatrix.at(1,3) = newCoordiantes.y;
		_positionMatrix.at(2,3) = newCoordiantes.z;
	}
}

Camera::CoordinatesLimit Camera::positionLimitsGet() const {
	return _positionLimits;
}

bool Camera::positionLimitsTest(const Coordinates& coordinates) const {
	return positionLimitsGet().test(coordinates);
}

void Camera::positionLimitsSet(const CoordinatesLimit& coordinatesLimit) {
	_positionLimits = coordinatesLimit;
}


void Camera::headRotationSetDefault(const Angles& degrees, RotationOrder order) {
	_defaultHeadRotationMatrix = TransformationMatrices::getRotationMatrix(-degrees.degreeX, degrees.degreeY, degrees.degreeZ, order);
	_defaultHeadRotationAngles = degrees;

}

void Camera::headRotationReset() {
	_headRotationMatrix = _defaultHeadRotationMatrix;
	_headRotationAngles = _defaultHeadRotationAngles;
}

void Camera::headRotationSet(const Angles& degrees, RotationOrder order) {
	if ( headRotationLimitsTest(degrees) ) {
		_headRotationMatrix = TransformationMatrices::getRotationMatrix(-degrees.degreeX, degrees.degreeY, degrees.degreeZ, order);
		_headRotationAngles = degrees;
	}
}

void Camera::headRotationRotate(const Angles& degrees, RotationOrder order) {
	Angles newAngles = _headRotationAngles + degrees;
	if ( headRotationLimitsTest(newAngles) ) {
		_headRotationMatrix = fmat44(TransformationMatrices::getRotationMatrix(-degrees.degreeX, degrees.degreeY, degrees.degreeZ, order) * _headRotationMatrix);
		_headRotationAngles = newAngles;
	}
}

Camera::AnglesLimit Camera::headRotationLimitsGet() const {
	return _headRotationLimits;
}

bool Camera::headRotationLimitsTest(const Angles& degrees) const {
	return headRotationLimitsGet().test(degrees);
}

void Camera::headRotationLimitsSet(const AnglesLimit& degreesLimit) {
	_headRotationLimits = degreesLimit;
}


void Camera::circlingDistanceSetDefault(const Coordinates& coordinates) {
	_defaultCirclingDistanceMatrix = TransformationMatrices::getTranslationMatrix(coordinates.x, coordinates.y, coordinates.z);
}

void Camera::circlingDistanceReset() {
	_circlingDistanceMatrix = _defaultCirclingDistanceMatrix;
}

void Camera::circlingDistanceSet(const Coordinates& coordinates) {
	if (circlingDistanceLimitsTest(coordinates))
		_circlingDistanceMatrix = TransformationMatrices::getTranslationMatrix(-coordinates.x, -coordinates.y, coordinates.z);
}

void Camera::circlingDistanceMove(const Coordinates& coordinates) {
	Coordinates newCoordiantes(_circlingDistanceMatrix.at(0,3) - coordinates.x, _circlingDistanceMatrix.at(1,3) - coordinates.y, _circlingDistanceMatrix.at(2,3) + coordinates.z);
	if (circlingDistanceLimitsTest(newCoordiantes)) {
		_circlingDistanceMatrix.at(0,3) = newCoordiantes.x;
		_circlingDistanceMatrix.at(1,3) = newCoordiantes.y;
		_circlingDistanceMatrix.at(2,3) = newCoordiantes.z;
	}
}

Camera::CoordinatesLimit Camera::circlingDistanceLimitsGet() const {
	return _circlingDistanceLimits;
}

bool Camera::circlingDistanceLimitsTest(const Coordinates& coordinates) const {
	return circlingDistanceLimitsGet().test(coordinates);
}

void Camera::circlingDistanceLimitsSet(const CoordinatesLimit& coordinatesLimit) {
	_circlingDistanceLimits = coordinatesLimit;
}


void Camera::circlingAroundSetDefault(const Angles& degrees, RotationOrder order) {
	_defaultCirclingAroundMatrix = TransformationMatrices::getRotationMatrix(degrees.degreeX, -degrees.degreeY, degrees.degreeZ, order);
	_defaultCirclingAroundAngles = degrees;

}

void Camera::circlingAroundReset() {
	_circlingAroundMatrix = _defaultCirclingAroundMatrix;
	_circlingAroundAngles = _defaultCirclingAroundAngles;
	_circlingAroundBasis = Camera::Coordinates(0, 0, 0);
}

void Camera::circlingAroundSet(const Angles& degrees, RotationOrder order) {
	if ( circlingAroundLimitsTest(degrees) ) {
		_circlingAroundMatrix = TransformationMatrices::getRotationMatrix(degrees.degreeX, -degrees.degreeY, degrees.degreeZ, order);
		_circlingAroundAngles = degrees;
	}
}

void Camera::circlingAroundCircle(const Angles& degrees, RotationOrder order) {
	Angles newAngles = _circlingAroundAngles + degrees;
	if ( circlingAroundLimitsTest(newAngles) ) {
		_circlingAroundMatrix = fmat44(TransformationMatrices::getRotationMatrix(degrees.degreeX, -degrees.degreeY, degrees.degreeZ, order) * _circlingAroundMatrix);
		_circlingAroundAngles = newAngles;
	}
}

void Camera::circlingAroundMoveBasis(const Coordinates& coordinates) {
	Coordinates newCoordiantes(_circlingAroundBasis.x - coordinates.x, _circlingAroundBasis.y - coordinates.y, _circlingAroundBasis.z + coordinates.z);
	if (circlingAroundBasisLimitsTest(newCoordiantes)) {
		_circlingAroundBasis = newCoordiantes;
		_circlingAroundMatrix = fmat44(TransformationMatrices::getTranslationMatrix(-coordinates.x, -coordinates.y, coordinates.z) * _circlingAroundMatrix);
	}
}

Camera::AnglesLimit Camera::circlingAroundLimitsGet() const {
	return _circlingAroundLimits;
}

bool Camera::circlingAroundLimitsTest(const Angles& degrees) const {
	return circlingAroundLimitsGet().test(degrees);
}

void Camera::circlingAroundLimitsSet(const AnglesLimit& degreesLimit) {
	_circlingAroundLimits = degreesLimit;
}

Camera::CoordinatesLimit Camera::circlingAroundBasisLimitsGet() const {
	return _circlingAroundBasisLimits;
}

bool Camera::circlingAroundBasisLimitsTest(const Coordinates& coordinates) const {
	return circlingAroundBasisLimitsGet().test(coordinates);
}

void Camera::circlingAroundBasisLimitsSet(const CoordinatesLimit& coordinatesLimit) {
	_circlingAroundBasisLimits = coordinatesLimit;
}


void Camera::zoomSetDefault(float zoom) {
	_defaultZoom = zoom;
}

void Camera::zoomReset() {
	_zoom = (float)_defaultZoom;
}

void Camera::zoomSet(float zoom) {
	if (zoomLimitsTest(zoom))
		_zoom = zoom;
}

void Camera::zoom(float zoom) {
	zoomMult(zoom);
}

void Camera::zoomMult(float zoom) {
	zoomSet(_zoom * zoom);
}

void Camera::zoomAdd(float zoom) {
	zoomSet(_zoom + zoom);
}

Camera::ZoomLimit Camera::zoomLimitsGet() const {
	return _zoomLimits;
}

bool Camera::zoomLimitsTest(float zoom) const {
	return zoomLimitsGet().test(zoom);
}

void Camera::zoomLimitsSet(ZoomLimit zoomLimit) {
	_zoomLimits = zoomLimit;
}


void Camera::resetAll() {
	headRotationReset();
	positionReset();
	circlingAroundReset();
	circlingDistanceReset();
	zoomReset();
	projectionReset();
}

Camera::Angles Camera::getHeadRotation() const {
	return _headRotationAngles;
}
fmat44 Camera::getHeadRotationMatrix() const {
	return _headRotationMatrix;
}

Camera::Coordinates Camera::getPosition() const {
	fmat44& m = getPositionMatrix();
	return Camera::Coordinates(m.at(0,3), m.at(1,3), m.at(2,3));
}
fmat44 Camera::getPositionMatrix() const {
	return _positionMatrix;
}

Camera::Angles Camera::getCirclingAround() const {
	return _circlingAroundAngles;
}
fmat44 Camera::getCirclingArounMatrix() const {
	return _circlingAroundMatrix;
}

Camera::Coordinates Camera::getCirclingDistance() const {
	fmat44& m = getCirclingDistanceMatrix();
	return Camera::Coordinates(m.at(0,3), m.at(1,3), m.at(2,3));
}
fmat44 Camera::getCirclingDistanceMatrix() const {
	return _circlingDistanceMatrix;
}

fmat44 Camera::getViewMatrix() const {
	//circlingDistance * circlingAround * position * headRotation
	//return getCirclingDistanceMatrix() * getCirclingArounMatrix() * getPositionMatrix() * getHeadRotationMatrix();
	return getCirclingDistanceMatrix() * getCirclingArounMatrix() * getHeadRotationMatrix() * getPositionMatrix();
}

float Camera::getZoom() const {
	return _zoom;
}

fmat44 Camera::getViewProjectionMatrix() const {
	return getProjectionMatrix() * getViewMatrix();
}


void Camera::lookAt(const Coordinates& lookFrom, const Coordinates& lookAt, float rotZdeg) {
	float lengthX = lookAt.x - lookFrom.x;
	float lengthY = lookAt.y - lookFrom.y;
	float lengthZ = lookAt.z - lookFrom.z;

	float rotY = 0.0f;
	if (lengthX != 0.0f) {
		rotY = atanf(abs(lengthZ / lengthX));
		rotY *= 180.0f / fdatum::pi;
		if (lengthX > 0 && lengthZ > 0) {
			rotY = 90.0f - rotY;
		} else if (lengthX > 0 && lengthZ < 0) {
			rotY = 90.0f + rotY;
		} else if (lengthX < 0 && lengthZ < 0) {
			rotY = 180.0f + (90.0f - rotY);
		} else if (lengthX < 0 && lengthZ > 0) {
			rotY = 270.0f + rotY;
		}
	}
	
	float rotX = 0.0f;
	if (lengthY != 0.0f) {
		rotX = atanf(abs(lengthZ / lengthY));
		rotX *= 180.0f / fdatum::pi;
		if (lengthZ > 0 && lengthY > 0) {
			rotX = 270.0f + rotX;
		}
		if (lengthZ > 0 && lengthY < 0) {
			rotX = 90.0f - rotX;
		}
		if (lengthZ < 0 && lengthY < 0) {
			rotX = 90.0f + rotX;
		}
		if (lengthZ < 0 && lengthY > 0) {
			rotX = 180.0f + (90.0f - rotX);
		}
	}
	/*float rotY = lengthX != 0.0f ? fdatum::pi*2 - atanf(lengthZ / lengthX) : 0.0f; rotY *= 180.0f / fdatum::pi;
	float rotX = lengthZ != 0.0f ? fdatum::pi*2 - atanf(lengthY / lengthZ) : 0.0f; rotX *= 180.0f / fdatum::pi;*/
	headRotationSet(Angles(-rotX, rotY, rotZdeg), RotationOrder::yxz);
	positionSet(lookFrom);
	circlingAroundSet(Angles(0.0f, 0.0f, 0.0f));
	circlingDistanceSet(Coordinates(0.0f, 0.0f, 0.0f));
}



void CameraPerspective::init() {
	Camera::init();
	projectionSetDefault();
	projectionReset();
}

void CameraPerspective::projectionSetDefault(const Projection& projection) {
	_defaultProjection = projection;
}

void CameraPerspective::projectionSet(const Projection& projection) {
	_projection = projection;
}

void CameraPerspective::projectionReset() {
	_projection = _defaultProjection;
}

fmat44 CameraPerspective::getProjectionMatrix() const {
	fmat44 scaleMatrix(fill::eye);
	fmat44 pMatrix(fill::zeros);
	float zoom = getZoom();
	float fovX = _projection.fovX / zoom * fdatum::pi / 180.0f;
	float fovY = _projection.fovY / zoom * fdatum::pi / 180.0f;
	//After we reach fovX/fovY of 180 degree - start scaling
	if (fovX >= fdatum::pi || fovY >= fdatum::pi) {
		float zoomP;
		float zoomScale;
		if (_projection.fovX > _projection.fovY) {
			zoomP = _projection.fovX / 179.9;
		} else {
			zoomP = _projection.fovY / 179.9;
		}
		fovX = _projection.fovX / zoomP * fdatum::pi / 180.0f;
		fovY = _projection.fovY / zoomP * fdatum::pi / 180.0f;
		zoomScale = zoom / zoomP;
		scaleMatrix = TransformationMatrices::getScaleMatrix(zoomScale);
	}
	float zNear = _projection.zNear;
	float zFar = _projection.zFar;
	float aspect = fovX / fovY;

	float f = 1 / tanf(fovY / 2);
	float A = (zFar + zNear) / (zNear - zFar);
	float B = (2 * zFar * zNear) / (zNear - zFar);

	pMatrix.at(0,0) = f / aspect;
	pMatrix.at(1,1) = f;
	pMatrix.at(2,2) = A;
	pMatrix.at(2,3) = B;
	pMatrix.at(3,2) = -1;

	return scaleMatrix * pMatrix;
}