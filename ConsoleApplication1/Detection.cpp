#include "Detection.h"
#include <time.h>
/*int x, int y, int height, int width*/
MyRectangle::MyRectangle(int x, int y, int height, int width)
		:xCoordonate(x), yCoordonate(y), height(height), width(width){}
/*{
	xCoordonate = x;
	yCoordonate = y;
	MyRectangle::height = height;
	MyRectangle::width = width;
}*/

/*MyRectangle::MyRectangle(const MyRectangle& copy_from) {
	xCoordonate = copy_from.xCoordonate;
	yCoordonate = copy_from.yCoordonate;
	height = copy_from.height;
	width = copy_from.width;
}

MyRectangle& MyRectangle::operator=(const MyRectangle& copy_from) {
	xCoordonate = copy_from.xCoordonate;
	yCoordonate = copy_from.yCoordonate;
	height = copy_from.height;
	width = copy_from.width;
}*/

int MyRectangle::getX() {

	return xCoordonate;
}

int MyRectangle::getY() {

	return yCoordonate; 
}

int MyRectangle::getHeight(){

	return height;
}

int MyRectangle::getWidth() {

	return width;
}

void MyRectangle::setHeight(int height) {

	MyRectangle::height = height;
}

void MyRectangle::setWidth(int width) {

	MyRectangle::width = width;
}

void MyRectangle::setY(int y) {

	yCoordonate = y;
}

void MyRectangle::setX(int x) {

	xCoordonate = x;
}

MyRectangle::~MyRectangle() {}

Detection::Detection(time_t initialTimestamp , time_t finalTimestamp, int x, int y , int height, int width ) {

	Detection::initialTimestamp = initialTimestamp;
	Detection::finalTimestamp = finalTimestamp;
	Detection::geometricDetection = new MyRectangle(x,y,height,width);
	Detection::facegeometricDetection = new MyRectangle(x, y, height, width);
	Detection::framesNotDetected = 0;
	Detection::apparitionNumber = 1;
	Detection::detectionIsHiding = false;
	Detection::ImpressionWasCounted = false;
	Detection::face = face;

}

void Detection::setGeometricRectangle(int x, int y, int height, int width) {

	geometricDetection->setX(x);
	geometricDetection->setY(y);
	geometricDetection->setHeight(height);
	geometricDetection->setWidth(width);
}

void Detection::setfaceGeometricRectangle(int initial_x, int initial_y, int initial_height, int initial_width) {

	facegeometricDetection->setX(initial_x);
	facegeometricDetection->setY(initial_y);
	facegeometricDetection->setHeight(initial_height);
	facegeometricDetection->setWidth(initial_width);
}

MyRectangle* Detection::getGeometricRectangle() {

	return geometricDetection;
}
MyRectangle* Detection::getfaceGeometricRectangle() {

	return facegeometricDetection;
}

time_t Detection::getInitialTimestamp() {

	return initialTimestamp;
}
time_t Detection::getFinalTimestamp() {

	return finalTimestamp;
}

void Detection::setInitialTimestamp(time_t TimestampI) {

	initialTimestamp = TimestampI;
}
void Detection::setFinalTimestamp(time_t TimestampF) {

	finalTimestamp = TimestampF;
}

void Detection::setFramesNotDetected(int numberFrames) {
	framesNotDetected = numberFrames;
}

int Detection::getFramesNotDetected() {
	return framesNotDetected;
}

bool Detection::isHiding() {
	return detectionIsHiding;
}

void Detection::isHiding(bool isDetectionHiding) {
	detectionIsHiding = isDetectionHiding;
}

void Detection::setApparitionNumber(int number) {
	apparitionNumber = number;		
}
int Detection::getApparitionNumber() {

	return apparitionNumber;
}

bool Detection::WasCounted() {
	return ImpressionWasCounted;
}

void Detection::WasCounted(bool Count) {

	Detection::ImpressionWasCounted = Count;
}

void Detection::setFace(cv::Mat face) {

	Detection::face = face;
}

cv::Mat Detection::getFace() {

	return Detection::face;
}
Detection::~Detection() {

}