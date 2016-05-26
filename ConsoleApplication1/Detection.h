#include <time.h>
#include "opencv2/imgproc/imgproc.hpp"
class MyRectangle {
public:
	MyRectangle(){}
	//MyRectangle(const MyRectangle& copy_from);
	//MyRectangle& MyRectangle::operator=(const MyRectangle& copy_from);
	MyRectangle(int, int, int, int);
	int getX();
	int getY();
	int getHeight();
	int getWidth();
	void setHeight(int);
	void setWidth(int);
	void setY(int);
	void setX(int);
	~MyRectangle();
private:
	/*x & y coordonate left and up of rectangle*/
	int xCoordonate;
	int yCoordonate;
	int height;
	int width;
};

class Detection {
public:
	/*high_resolution_clock::time_point initialTimestamp, high_resolution_clock::time_point finalTimestamp, int x, int y, int height, int width*/
	Detection(time_t initialTimestamp, time_t finalTimestamp, int x, int y, int height, int width);
	time_t getInitialTimestamp();
	time_t getFinalTimestamp();
	void setInitialTimestamp(time_t TimestampI);
	void setFinalTimestamp(time_t TimestampF);
	void setGeometricRectangle(int x, int y, int height, int width);
	void setfaceGeometricRectangle(int initial_x, int initial_y, int initial_height, int initial_width);
	void setFramesNotDetected(int numberFrames);
	int getFramesNotDetected();
	bool isHiding();
	void isHiding(bool isDetectionHiding);
	bool WasCounted();
	void WasCounted(bool Count);
	MyRectangle* getGeometricRectangle();
	MyRectangle* getfaceGeometricRectangle();
	void setApparitionNumber(int number);
	int getApparitionNumber();
	void setFace(cv::Mat face);
	cv::Mat getFace();
	~Detection();
private:
	time_t initialTimestamp;
	time_t finalTimestamp;
	MyRectangle *geometricDetection;
	MyRectangle *facegeometricDetection;
	/*How many frames passed and object was not detected*/
	int framesNotDetected;
	/*For the moment the detection is hiding*/
	bool detectionIsHiding;
	int apparitionNumber;
	bool ImpressionWasCounted;
	cv::Mat face;
	Detection(){}
};