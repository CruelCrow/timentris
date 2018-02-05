#include "Piece.h"

Piece::metrics Piece::getMetrics(const Mat<int> &m) {
	//Use Armadillo min/max for matrices: min(mat, dim=0) / max(mat, dim=0)
	if (m.n_cols < 1 || (m.n_rows != 4 && m.n_rows != 5)) {
		throw PieceExceptions::WrongMatrixSize();
	}
	metrics retMetrics;
	Col<int> mins = arma::min(m, 1);
	Col<int> maxs = arma::max(m, 1);

	retMetrics.minX = mins[0];
	retMetrics.maxX = maxs[0];

	retMetrics.minY = mins[1];
	retMetrics.maxY = maxs[1];

	retMetrics.minZ = mins[2];
	retMetrics.maxZ = maxs[2];

	retMetrics.minW = mins[3];
	retMetrics.maxW = maxs[3];

	retMetrics.sizeX = retMetrics.maxX - retMetrics.minX + 1;
	retMetrics.sizeY = retMetrics.maxY - retMetrics.minY + 1;
	retMetrics.sizeZ = retMetrics.maxZ - retMetrics.minZ + 1;
	retMetrics.sizeW = retMetrics.maxW - retMetrics.minW + 1;

	return retMetrics;
}

Piece::metrics Piece::getMetrics(const vector<Brick> &vb) {
	return getMetrics(PieceHelper::bricksToMatrix(vb));
}

Piece::metrics Piece::getMetrics(const Piece &p) {
	return getMetrics(PieceHelper::bricksToMatrix(p._originalBricks));
}

Mat<int> Piece::getNormalizedMatrix(const Mat<int> &m) {
	//TODO: this algorythm is wrong, write another one
	if (m.n_cols < 1 || (m.n_rows != 4 && m.n_rows != 5)) {
		throw PieceExceptions::WrongMatrixSize();
	}
	Mat<int> localM(m);
	if (m.n_rows == 4) {
		//add fifth row
		localM.resize(5, m.n_cols);
		for (unsigned int i = 0; i < m.n_cols; i++) localM(4,i) = 1; //set fifth row to 1
	}
	metrics originalMetrics = Piece::getMetrics(m);
	int dX = (int)ceil(((double)originalMetrics.maxX + (double)originalMetrics.minX) / 2) * (-1);
	int dY = (int)ceil(((double)originalMetrics.maxY + (double)originalMetrics.minY) / 2) * (-1);
	int dZ = (int)ceil(((double)originalMetrics.maxZ + (double)originalMetrics.minZ) / 2) * (-1);
	int dW = (int)ceil(((double)originalMetrics.maxW + (double)originalMetrics.minW) / 2) * (-1);
	Mat<int> tMatrix(5,5); //matrix of shifts only
	tMatrix << 1 << 0 << 0 << 0 << dX << endr
			<< 0 << 1 << 0 << 0 << dY << endr
			<< 0 << 0 << 1 << 0 << dZ << endr
			<< 0 << 0 << 0 << 1 << dW << endr
			<< 0 << 0 << 0 << 0 << 1 << endr
			;
	return tMatrix * localM;
}

Mat<int> Piece::getNormalizedMatrix(const vector<Brick> &vb) {
	return getNormalizedMatrix(PieceHelper::bricksToMatrix(vb));
}

Mat<int> Piece::getNormalizedMatrix(const Piece &p) {
	return getNormalizedMatrix(PieceHelper::bricksToMatrix(p._originalBricks));
}


void Piece::init(void) {
	_dX = _dY = _dZ = _dW = _aXY = _aXZ = _aXW = _aYZ = _aYW = _aZW = 0; //no shifts or rotations
	setDifficultyLevel(0); //default difficulty level of a Piece is 0
}

Piece::Piece(void)
{
	init();
}
Piece::Piece(const vector<Brick> &bricks) {
	init();

	for (unsigned int i = 0; i < bricks.size(); i++) {
		addBrick(bricks[i]);
	}
}
Piece::Piece(const Mat<int> &mBricks) {
	init();

	vector<Brick> vBricks = PieceHelper::matrixToBricks(mBricks);
	for (unsigned int i = 0; i < vBricks.size(); i++) {
		addBrick(vBricks[i]);
	}
}
Piece::Piece(const string filename) {
	init();

	tinyxml2::XMLDocument* xmlPieceDoc = new tinyxml2::XMLDocument();
	xmlPieceDoc->LoadFile(filename.c_str());
	if (xmlPieceDoc->Error()) {
		delete xmlPieceDoc;
		throw PieceExceptions::WrongFileStructure();
	}
	XMLElement* xmlPieceElement = xmlPieceDoc->FirstChildElement("piece");
	if (!xmlPieceElement) {
		delete xmlPieceDoc;
		throw PieceExceptions::WrongFileStructure();
	}
	if ( //read root element basic attributes
		!xmlPieceElement->Attribute("difficultyLevel")
		||
		!xmlPieceElement->Attribute("dX")
		||
		!xmlPieceElement->Attribute("dY")
		||
		!xmlPieceElement->Attribute("dZ")
		||
		!xmlPieceElement->Attribute("dW")
		||
		!xmlPieceElement->Attribute("aXY")
		||
		!xmlPieceElement->Attribute("aXZ")
		||
		!xmlPieceElement->Attribute("aXW")
		||
		!xmlPieceElement->Attribute("aYZ")
		||
		!xmlPieceElement->Attribute("aYW")
		||
		!xmlPieceElement->Attribute("aZW")
		) {
		delete xmlPieceDoc;
		throw PieceExceptions::WrongFileStructure();
	}
	_difficultyLevel = atoi(xmlPieceElement->Attribute("difficultyLevel"));
	_dX = atoi(xmlPieceElement->Attribute("dX"));
	_dY = atoi(xmlPieceElement->Attribute("dY"));
	_dZ = atoi(xmlPieceElement->Attribute("dZ"));
	_dW = atoi(xmlPieceElement->Attribute("dW"));
	_aXY = atoi(xmlPieceElement->Attribute("aXY"));
	_aXW = atoi(xmlPieceElement->Attribute("aXW"));
	_aYZ = atoi(xmlPieceElement->Attribute("aYZ"));
	_aYW = atoi(xmlPieceElement->Attribute("aYW"));
	_aZW = atoi(xmlPieceElement->Attribute("aZW"));
	//now read bricks matrix (CDATA from piece->bricks)
	XMLElement* xmlBricksElement = xmlPieceDoc->FirstChildElement("piece")->FirstChildElement("bricks"); //load root element
	if (!xmlBricksElement) {
		delete xmlPieceDoc;
		throw PieceExceptions::WrongFileStructure();
	}
	//create matrix object from a string
	Mat<int> mBricks(xmlBricksElement->GetText());
	//convert matrix to avector of bricks
	vector<Brick> vBricks = PieceHelper::matrixToBricks(mBricks);
	//save bricks
	for (unsigned int i = 0; i < vBricks.size(); i++) {
		addBrick(vBricks[i]);
	}
	delete xmlPieceDoc;
}

void Piece::setDifficultyLevel(unsigned int difficultyLevel) {
	_difficultyLevel = difficultyLevel;
}
unsigned int Piece::getDifficultyLevel() {
	return _difficultyLevel;
}

void Piece::addBrick(const Brick &brick) {
	//if brick exists => throw exception
	if (isBrickExists(brick)) {
		throw PieceExceptions::BrickAlreadyExists();
	}
	_originalBricks.push_back(brick);
	return;
}
void Piece::addBrick(int x, int y, int z, int w) {
	addBrick(Brick(x, y, z, w));
}

void Piece::removeBrick(const Brick &brick) {
	if (!isBrickExists(brick)) {		
		throw PieceExceptions::BrickNotFound();
	}
	for (unsigned int i = 0; i < _originalBricks.size(); i++) {
		if (_originalBricks[i] == brick) {
			//remove brick
			_originalBricks.erase(_originalBricks.begin() + i);
			return;
		}
	}
	return;
}
void Piece::removeBrick(int x, int y, int z, int w) {
	removeBrick(Brick(x, y, z, w));
}

void Piece::removeAllBricks() {
	_originalBricks.clear();
}

bool Piece::isBrickExists(const Brick &brick) const {
	for (unsigned int i = 0; i < _originalBricks.size(); i++) {
		if (_originalBricks[i] == brick) {
			return true;
		}
	}
	return false;
}
bool Piece::isBrickExists(int x, int y, int z, int w) const {
	return isBrickExists(Brick(x, y, z, w));
}

unsigned int Piece::getNumberOfBricks() const {
	return _originalBricks.size();
}


void Piece::rotate(int aXY, int aXZ, int aXW, int aYZ, int aYW, int aZW) {
	if (aXY % 90 != 0 || aXZ % 90 != 0 || aXW % 90 != 0 || aYZ % 90 != 0 || aYW % 90 != 0 || aZW % 90 != 0) {
		throw PieceExceptions::WrongRotationAngle();
	}
	 _aXY = (_aXY + aXY) % 360;
	 _aXZ = (_aXZ + aXZ) % 360;
	 _aXW = (_aXW + aXW) % 360;
	 _aYZ = (_aYZ + aYZ) % 360;
	 _aYW = (_aYW + aYW) % 360;
	 _aZW = (_aZW + aZW) % 360;
}

void Piece::rotateXY() {
	rotate(90);
}

void Piece::rotateXZ() {
	rotate(0,90);
}

void Piece::rotateXW() {
	rotate(0,0,90);
}

void Piece::rotateYZ() {
	rotate(0,0,0,90);
}

void Piece::rotateYW() {
	rotate(0,0,0,0,90);
}

void Piece::rotateZW() {
	rotate(0,0,0,0,0,90);
}

void Piece::clearRotations() {
	_aXY = _aXZ = _aXW = _aYZ = _aYW = _aZW = 0;
}


void Piece::move(int dX=0, int dY=0, int dZ=0, int dW=0) {
	_dX += dX;
	_dY += dY;
	_dZ += dZ;
	_dW += dW;
}
void Piece::moveX(int dX = 1) {
	move(dX);
}
void Piece::moveY(int dY = 1) {
	move(0,dY);
}
void Piece::moveZ(int dZ = 1) {
	move(0,0,dZ);
}
void Piece::moveW(int dW = 1) {
	move(0,0,0,dW);
}
void Piece::clearShifts() {
	_dX = _dY = _dZ = _dW = 0;
}

bool Piece::saveToFile(const string filename) const {
	tinyxml2::XMLDocument* xmlDoc = new tinyxml2::XMLDocument();
	XMLDeclaration* xmlDecl = xmlDoc->NewDeclaration("<?xml version=\"1.1\" encoding=\"UTF-8\" ?>");
	xmlDoc->LinkEndChild(xmlDecl);
	XMLElement* xmlPiece = xmlDoc->NewElement("piece");
		xmlPiece->SetAttribute("difficultyLevel", _difficultyLevel);
		xmlPiece->SetAttribute("dX", _dX);
		xmlPiece->SetAttribute("dY", _dY);
		xmlPiece->SetAttribute("dZ", _dZ);
		xmlPiece->SetAttribute("dW", _dW);
		xmlPiece->SetAttribute("aXY", _aXY);
		xmlPiece->SetAttribute("aXZ", _aXZ);
		xmlPiece->SetAttribute("aXW", _aXW);
		xmlPiece->SetAttribute("aYZ", _aYZ);
		xmlPiece->SetAttribute("aYW", _aYW);
		xmlPiece->SetAttribute("aZW", _aZW);
		XMLElement* xmlBricks = xmlDoc->NewElement("bricks");
			std::stringstream bricksMatrix;
			bricksMatrix << PieceHelper::bricksToMatrix(_originalBricks); //stream matrix to string stream
			std::string bricksMatrixStr(bricksMatrix.str()); //save string stream as a simple string
			replace(bricksMatrixStr.begin(), bricksMatrixStr.end(), '\n', ';'); //replace \n to ;
			XMLText* xmlBricksCDATA = xmlDoc->NewText(bricksMatrixStr.c_str()); //save to CDATA element
				xmlBricksCDATA->SetCData(true);
				xmlBricks->LinkEndChild(xmlBricksCDATA);
			xmlPiece->LinkEndChild(xmlBricks);
	xmlDoc->LinkEndChild(xmlPiece);
	xmlDoc->SaveFile(filename.c_str()); //save to file
	bool success = !xmlDoc->Error();
	delete xmlDoc;
	return success;
}

Mat<int> Piece::calculateRotationMatrix() const {
	//Formula found at: http://ken-soft.com/2009/01/08/graph4d-rotation4d-project-to-2d/

	//For each rotation "axis" calculate transfomration matrix for rotations and mutiply it one by another
	Mat<int> tMatrix(5,5); //matrix of rotations only
	//init with an identity matrix
	tMatrix 
		<< 1 << 0 << 0 << 0 << 0 << endr
		<< 0 << 1 << 0 << 0 << 0 << endr
		<< 0 << 0 << 1 << 0 << 0 << endr
		<< 0 << 0 << 0 << 1 << 0 << endr
		<< 0 << 0 << 0 << 0 << 1 << endr
		;

	//rotate _aXY
	double _aXYr = _aXY * arma::math::pi() / 180.0; 
	Mat<int> xyMatrix(5,5);
	xyMatrix
		<< (int)cos(_aXYr) << (int)sin(_aXYr) << 0 << 0 << 0 << endr
		<< -(int)sin(_aXYr) << (int)cos(_aXYr) << 0 << 0 << 0 << endr
		<< 0 << 0 << 1 << 0 << 0 << endr
		<< 0 << 0 << 0 << 1 << 0 << endr
		<< 0 << 0 << 0 << 0 << 1 << endr
		;

	//rotate _aXZ
	double _aXZr = _aXZ * arma::math::pi() / 180.0; 
	Mat<int> xzMatrix(5,5);
	xzMatrix
		<< (int)cos(_aXZr) << 0 << -(int)sin(_aXZr) << 0 << 0 << endr
		<< 0 << 1 << 0 << 0 << 0 << endr
		<< (int)sin(_aXZr) << 0 << (int)cos(_aXZr) << 0 << 0 << endr
		<< 0 << 0 << 0 << 1 << 0 << endr
		<< 0 << 0 << 0 << 0 << 1 << endr
		;

	//rotate _aXW
	double _aXWr = _aXW * arma::math::pi() / 180.0; 
	Mat<int> xwMatrix(5,5);
	xwMatrix 
		<< (int)cos(_aXWr) << 0 << 0 << (int)sin(_aXWr) << 0 << endr
		<< 0 << 1 << 0 << 0 << 0 << endr
		<< 0 << 0 << 1 << 0 << 0 << endr
		<< -(int)sin(_aXWr) << 0 << 0 << (int)cos(_aXWr) << 0 << endr
		<< 0 << 0 << 0 << 0 << 1 << endr
		;

	//rotate _aYZ
	double _aYZr = _aYZ * arma::math::pi() / 180.0; 
	Mat<int> yzMatrix(5,5);
	yzMatrix 
		<< 1 << 0 << 0 << 0 << 0 << endr
		<< 0 << (int)cos(_aYZr) << (int)sin(_aYZr) << 0 << 0 << endr
		<< 0 << -(int)sin(_aYZr) << (int)cos(_aYZr) << 0 << 0 << endr
		<< 0 << 0 << 0 << 1 << 0 << endr
		<< 0 << 0 << 0 << 0 << 1 << endr
		;

	//rotate _aYW
	double _aYWr = _aYW * arma::math::pi() / 180.0; 
	Mat<int> ywMatrix(5,5);
	ywMatrix 
		<< 1 << 0 << 0 << 0 << 0 << endr
		<< 0 << (int)cos(_aYWr) << 0 << -(int)sin(_aYWr) << 0 << endr
		<< 0 << 0 << 1 << 0 << 0 << endr
		<< 0 << (int)sin(_aYWr) << 0 << (int)cos(_aYWr) << 0 << endr
		<< 0 << 0 << 0 << 0 << 1 << endr
		;

	//rotate _aZW
	double _aZWr = _aZW * arma::math::pi() / 180.0; 
	Mat<int> zwMatrix(5,5);
	zwMatrix 
		<< 1 << 0 << 0 << 0 << 0 << endr
		<< 0 << 1 << 0 << 0 << 0 << endr
		<< 0 << 0 << (int)cos(_aZWr) << -(int)sin(_aZWr) << 0 << endr
		<< 0 << 0 << (int)sin(_aZWr) << (int)cos(_aZWr) << 0 << endr
		<< 0 << 0 << 0 << 0 << 1 << endr
		;

	tMatrix = xyMatrix * xzMatrix * xwMatrix * yzMatrix * ywMatrix * zwMatrix;
	return tMatrix;
}

Mat<int> Piece::calculateShiftMatrix() const {
	//For each shift (x,y,z,w) calculate transfomration matrix for shifts and mutiply it one by another
	Mat<int> tMatrix(5,5); //matrix of shifts only
	tMatrix << 1 << 0 << 0 << 0 << _dX << endr
			<< 0 << 1 << 0 << 0 << _dY << endr
			<< 0 << 0 << 1 << 0 << _dZ << endr
			<< 0 << 0 << 0 << 1 << _dW << endr
			<< 0 << 0 << 0 << 0 << 1 << endr
			;
	return tMatrix;
}

Piece::metrics Piece::getOrigianlMetrics() const {
	return Piece::getMetrics(*this);
}

Piece::metrics Piece::getTransformedMetrics() const {
	return Piece::getMetrics(getTransformedBricks());
}

const vector<Brick>& Piece::getOriginalBricks() const {
	return _originalBricks;
}

vector<Brick> Piece::getTransformedBricks() const {
	return
		PieceHelper::matrixToBricks(
			calculateShiftMatrix() * getNormalizedMatrix(
											calculateRotationMatrix() * getNormalizedMatrix(*this)
											)
			);
}


bool Piece::operator== (const Piece &p) const {
	return PieceHelper::comapreBricksVectors(_originalBricks, p._originalBricks);
}
 
bool Piece::operator!= (const Piece &p) const {
    return !(*this == p);
}

Piece& operator<< (Piece &p, const Brick &b) {
	p.addBrick(b);
	return p;
}