//** FUNCTION VIDEO MODULE */

#include "opencvheader.h"
#include "Detection.h"


cv::VideoCapture openVideo();
void detectAndDisplay(cv::Mat);
int RunVideo();
void getListCameras(std::vector<int> &);
void getSupportedResolution(cv::VideoCapture& capture, std::map<int, int> &);
cv::VideoCapture StartCameraConfiguration();
void ConfigureParameters();
inline void getCurrentHourMin(int &, int &);
inline void getCurrentHourMinSec(int&, int&, int&);
inline void getCurrentHourMinFromConf(int &, int &);
void sendLogFiles();
bool timeToSendLogFiles();