#include <chrono>
using namespace std::chrono;

#include "params.h"
#include "VideoModule.h"
#include "NetworkModule.h"
#include "currentParams.h"

high_resolution_clock::time_point A;
high_resolution_clock::time_point B;
high_resolution_clock::time_point C;
high_resolution_clock::time_point D;
extern double current_SCALEFACTOR;
extern int current_MINIMUMNEIGBOURS;
extern int current_minXSizeSE;
extern int current_minYSizeSE;
extern int current_MINIMUMFACEAPPARITION;
extern int current_DISTANCEFACETOBEDETECTEDX;
extern int current_DISTANCEFACETOBEDETECTEDY;
extern int current_FRAMESUNTILDELETEDETECTION;
extern std::string current_WINDOWNAME;
extern int current_HOURTOSENDLOGFILES;
extern int current_HOURTOSENDLOGFILESRESET;
extern bool todayLogFilesWasSent;
extern int current_MINTOSENDLOGFILES;
extern unsigned __int32 current_secToSendKeepAlive;
extern std::string LogFilesPath;
extern int current_minutesToSendLogFile;

int hourStartKeepAlive = 0, minStartKeepAlive = 0, secStartKeepAlive = 0;
int hourStartSendLogs = 0, minStartSendLogs = 0;

cv::String face_cascade_name = default_TRAINER_OUR_PROGRAM;
cv::CascadeClassifier face_cascade;
size_t currentFacesNumber = 0;
std::vector<Detection> baseDetections;
int Temporalthreshold = 0;
int Impression = 0;
int SentImpressionToServer = 0;
long long durationtwo = 0;


/** Get list of all cameras available*/
void getListCameras(std::vector<int> &list) {
	for (int i = 0; i < 10; i++) {

		cv::VideoCapture capture(i);
		if (capture.isOpened()) {

			list.push_back(i);
		}
		
		capture.~VideoCapture();
	}
}

/**VideoCapture: device, and a parameters in which the function will put the list of
supported resolution
cv::VideoCapture& capture, std::map<int, int> &finalListRezolution*/
void getSupportedResolution(cv::VideoCapture& capture, std::map<int, int> &finalListRezolution) {


	std::vector<std::pair<int, int>> rezolutionList;
	rezolutionList.emplace_back(120, 160);
	rezolutionList.emplace_back(160, 240);
	rezolutionList.emplace_back(200, 320);
	rezolutionList.emplace_back(240, 320);
	rezolutionList.emplace_back(240, 360);
	rezolutionList.emplace_back(240, 384);
	rezolutionList.emplace_back(240, 400);
	rezolutionList.emplace_back(240, 432);
	rezolutionList.emplace_back(320, 480);
	rezolutionList.emplace_back(360, 480);
	rezolutionList.emplace_back(480, 640);
	rezolutionList.emplace_back(600, 800);
	rezolutionList.emplace_back(720, 960);
	rezolutionList.emplace_back(720, 1152);
	rezolutionList.emplace_back(720, 1280);
	rezolutionList.emplace_back(800, 1280);
	rezolutionList.emplace_back(900, 1440);
	rezolutionList.emplace_back(960, 1280);
	rezolutionList.emplace_back(960, 1600);
	rezolutionList.emplace_back(1152, 2048);
	rezolutionList.emplace_back(1200, 1600);
	rezolutionList.emplace_back(1280, 1920);
	rezolutionList.emplace_back(1344, 1856);
	rezolutionList.emplace_back(1440, 1920);
	rezolutionList.emplace_back(1440, 2160);
	rezolutionList.emplace_back(1440, 2560);
	rezolutionList.emplace_back(1600, 2560);
	rezolutionList.emplace_back(2400, 3200);
	rezolutionList.emplace_back(900, 1440);

	/*HEIGHT AND WIDTH*/
	finalListRezolution.insert(std::pair<int, int>(0, 0));
	std::pair<std::map<int, int>::iterator, bool> ret;
	std::cout << "Please wait. \nDetect supported resolution:" << std::endl;
	//int trigger = 0;
	//int captureSize = 0;
	double frameInitialWidth = capture.get(CV_CAP_PROP_FRAME_WIDTH);

	double frameInitialHeight = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
	for (unsigned int i = 1; i <= rezolutionList.size(); ++i) {
		if (rezolutionList[i].first > frameInitialHeight) {
			continue;
		}
		if (rezolutionList[i].second > frameInitialWidth) {
			continue;
		}
		Sleep(100);

		capture.set(CV_CAP_PROP_FRAME_HEIGHT, rezolutionList[i].first);
		capture.set(CV_CAP_PROP_FRAME_WIDTH, rezolutionList[i].second);
		int x = int(capture.get(CV_CAP_PROP_FRAME_WIDTH));
		int y = int(capture.get(CV_CAP_PROP_FRAME_HEIGHT));
		std::cout << ".";
		ret = finalListRezolution.insert(std::pair<int, int>(y, x));
		//	if (ret.second == false) {

		//trigger++;
		//}
		//else {

		//trigger = 0;
		//captureSize++;
		//	}
		//if ((trigger == 8) && (captureSize>0))
		//	break;
		//}
	}
		finalListRezolution.erase(finalListRezolution.begin());

		rezolutionList.~vector();
}


/// <summary>
/// Detect and Dysplay
/// </summary>
inline void detectAndDisplay(cv::Mat frame) {
	
	std::vector<cv::Rect> faces;
	cv::Mat frame_gray;
	
	time_t t1;
	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);
	//cv::Mat newww;
//	frame_gray.setTo(cv::Scalar(0, 0, 0));
	//frame_gray.convertTo(newww, -1, 0.5, 0);
	//-- Detect faces
	//imshow(current_WINDOWNAME, newww);
	std::cout << "sizeofFrameSend: " << sizeof(frame_gray) << std::endl;
	B = high_resolution_clock::now();
	
	face_cascade.detectMultiScale(frame_gray, faces, current_SCALEFACTOR, current_MINIMUMNEIGBOURS, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(current_minXSizeSE, current_minYSizeSE));
	C = high_resolution_clock::now();
	
	//sumi += durationtwo;
	//iii++;
	
	/*Get timestamp*/
	time(&t1);
	////////////////////////////////////////////////////

	for (int currentDetectionBaseNumber = 0; currentDetectionBaseNumber < baseDetections.size(); currentDetectionBaseNumber++) {
		baseDetections[currentDetectionBaseNumber].isHiding(true);
		if ((baseDetections[currentDetectionBaseNumber].getApparitionNumber() > current_MINIMUMFACEAPPARITION) && (!baseDetections[currentDetectionBaseNumber].WasCounted())) {
			baseDetections[currentDetectionBaseNumber].WasCounted(true);
			++Impression;
		}
	}

	/*if new detection put on baseDetections*/
	
	for (int i = 0/*anteFace.size()*/; i < faces.size(); i++) {
		bool newfaceFind = true;
		/*Verify if the face is in vectorbase*/
		for (int numberDetectie = 0; numberDetectie < baseDetections.size(); ++numberDetectie) {

			if (abs(faces[i].x - baseDetections[numberDetectie].getGeometricRectangle()->getX()) < current_DISTANCEFACETOBEDETECTEDX) {
				if (abs(faces[i].y - baseDetections[numberDetectie].getGeometricRectangle()->getY()) > current_DISTANCEFACETOBEDETECTEDY) {
					newfaceFind = true;
					continue;
				}
				newfaceFind = false;

				//de revazut debug
				baseDetections[numberDetectie].isHiding(false);
				baseDetections[numberDetectie].setGeometricRectangle(faces[i].x, faces[i].y, faces[i].height, faces[i].width);

				/*actualizare timestamp daca nu e noua*/
				baseDetections[numberDetectie].setFinalTimestamp(t1);
				int number = baseDetections[numberDetectie].getApparitionNumber();

				if (number == current_MINIMUMFACEAPPARITION) {

					cv::Rect myROI(faces[i].x, faces[i].y, faces[i].width, faces[i].height);
					cv::Mat croppedFaceInter = frame(myROI);
					cv::Mat croppedFace;
					croppedFaceInter.copyTo(croppedFace);
					baseDetections[numberDetectie].setFace(croppedFace);
					baseDetections[numberDetectie].setfaceGeometricRectangle(faces[i].x, faces[i].y, faces[i].height, faces[i].width);
				}

				baseDetections[numberDetectie].setApparitionNumber(++number);

				
				/*If it hides that means it come back*/
				baseDetections[numberDetectie].setFramesNotDetected(0);
				break;
			}

		}
		if (newfaceFind) {
			/////////////////////////
			////////////////////////
			Detection *face = new Detection(t1, t1, faces[i].x, faces[i].y, faces[i].height, faces[i].width);
			baseDetections.push_back(*face);
		}
	}	
	
	int numar= 0;

	/*For each Detection*/
	/*For each frame we increment threshold frames if the detection is hiding from us*/
	/*Send server && Maintanence for hiding faces*/
	for (int numberDetectie = 0; numberDetectie < baseDetections.size(); ++numberDetectie) {

		//auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(baseDetections[numberDetectie].getFinalTimestamp() - baseDetections[numberDetectie].getInitialTimestamp()).count();
		//std::cout << "Numar detectie din base vector" << numberDetectie << "  " << diff << "  " << baseDetections[numberDetectie].getFramesNotDetected() << " x: " << baseDetections[numberDetectie].getGeometricRectangle()->getX() << "  y: " << baseDetections[numberDetectie].getGeometricRectangle()->getY() << " Nr ap " << baseDetections[numberDetectie].getApparitionNumber() << std::endl;
		time_t diff = baseDetections[numberDetectie].getFinalTimestamp() - baseDetections[numberDetectie].getInitialTimestamp();
		std::cout << "Numar detectie din base vector" << numberDetectie << "  " << diff << "s  " << baseDetections[numberDetectie].getFramesNotDetected() << " x: " << baseDetections[numberDetectie].getGeometricRectangle()->getX() << "  y: " << baseDetections[numberDetectie].getGeometricRectangle()->getY() << " Nr ap " << baseDetections[numberDetectie].getApparitionNumber() << std::endl;
		if (baseDetections[numberDetectie].isHiding() == true) {

			/*if detection ready to go*/
			if (baseDetections[numberDetectie].getFramesNotDetected() > current_FRAMESUNTILDELETEDETECTION) {

				auto diff = baseDetections[numberDetectie].getFinalTimestamp() - baseDetections[numberDetectie].getInitialTimestamp();
				if (baseDetections[numberDetectie].getApparitionNumber() > current_MINIMUMFACEAPPARITION) {

					std::cout << "Detection send to server: time: " << diff << std::endl;
					time_t tt = baseDetections[numberDetectie].getInitialTimestamp();
					int xToSend = baseDetections[numberDetectie].getfaceGeometricRectangle()->getX();
					int yToSend = baseDetections[numberDetectie].getfaceGeometricRectangle()->getY();
					int heightToSend = baseDetections[numberDetectie].getfaceGeometricRectangle()->getHeight();
					int WidthToSend = baseDetections[numberDetectie].getfaceGeometricRectangle()->getWidth();

					std::thread(postImpression, diff, tt, xToSend, yToSend, heightToSend, WidthToSend, baseDetections[numberDetectie].getFace()).detach();

					SentImpressionToServer++;
				}
				
				/*Delete detection if it was send to server*/
				baseDetections.erase(baseDetections.begin() + numberDetectie);
				--numberDetectie;
				continue;
			}
			int temporarFrames;
			temporarFrames = baseDetections[numberDetectie].getFramesNotDetected();
			baseDetections[numberDetectie].setFramesNotDetected(++temporarFrames);
		}
	}

	for (int currentDetectionBaseNumber = 0; currentDetectionBaseNumber < baseDetections.size(); currentDetectionBaseNumber++) {

		if (baseDetections[currentDetectionBaseNumber].isHiding() == false) {
			numar++;
		}
	}

	std::cout << "Numar detectii " << numar << std::endl;
	std::cout << std::endl;
	std::cout << "Impressions " << Impression << std::endl;
	std::cout << "Impressions sent to server " << SentImpressionToServer << std::endl;
	std::cout << std::endl;
	
	
//	++ServerKeepAlive;

	int hourFinishKeepAlive, minFinishKeepAlive, secFinishKeepAlive;
	getCurrentHourMinSec(hourFinishKeepAlive, minFinishKeepAlive, secFinishKeepAlive);
	unsigned __int32 hourToSec = (hourFinishKeepAlive - hourStartKeepAlive) * 3600;
	int MintoSec = (minFinishKeepAlive - minStartKeepAlive) * 60;
	unsigned __int32 pastSeconds = hourToSec + MintoSec + (secFinishKeepAlive - secStartKeepAlive);



	if (pastSeconds >= current_secToSendKeepAlive) {

		std::thread (postKeepAlive).detach();
		hourStartKeepAlive = hourFinishKeepAlive;
		minStartKeepAlive = minFinishKeepAlive;
		secStartKeepAlive = secFinishKeepAlive;
		std::cout << "SendKeepAlive..." << std::endl;
	}
	//std::cout << "NM:" << durationtwo << " ";
	/////////////////////////////////////////////////////

	if ((current_displayVideo == 1) && (current_drawDetection == 1)) {

		for (size_t i = 0; i < faces.size(); i++) {

			cv::Point center(int(faces[i].x + faces[i].width*0.5), int(faces[i].y + faces[i].height*0.5));
			cv::ellipse(frame, center, cv::Size(int(faces[i].width*0.5), int(faces[i].height*0.5)), 0, 0, 360, cv::Scalar(255, 0, 255), 4, 8, 0);
		}
	}
	//-- Show what you got
	
	if (current_displayVideo == 1)
		imshow(current_WINDOWNAME, frame);
	
	
}

int RunVideo() {

	//cv::VideoCapture capture("vid7.mp4");
	cv::VideoCapture capture(sourceVideo);
	if (!capture.isOpened()) {
		printf("capture %s is not opened.", sourceVideo);
		return 1;
	}
	//capture.set(cv::CAP_PROP_FRAME_WIDTH, double(current_USER_CAP_PROP_FRAME_WIDTH));
	//printf("//Width of the frames in the video stream\n.");

	//capture.set(cv::CAP_PROP_FRAME_HEIGHT, double(current_USER_CAP_PROP_FRAME_HEIGHT));
	//printf("//Height of the frames in the video stream.\n");


	cv::Mat frame;
	cv::Mat outputframe;
	if (!face_cascade.load(face_cascade_name)) {

		std::cout<<"--(!)Error loadingm face cascade\n";
		return -1;
	}
	
	//-- 2. Read the video stream
	capture >> frame;
	std::vector<cv::Rect> faces;
	time_t t1;
	cv::Mat frame_gray;
	//////////////////////////////////////////////////
	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);
	face_cascade.detectMultiScale(frame_gray, faces, current_SCALEFACTOR, current_MINIMUMNEIGBOURS, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(current_minXSizeSE, current_minYSizeSE), cv::Size(default_maxXSizeSE, default_maxYSizeSE));
	time(&t1);
	currentFacesNumber = faces.size();
	
	if (currentFacesNumber > 0) {
		for (int i = 0; i < faces.size(); i++) {
			Detection *face = new Detection(t1, t1, faces[i].x, faces[i].y, faces[i].height, faces[i].width);
			baseDetections.push_back(*face);
			delete face;
		}
	}
	faces.~vector();
	frame_gray.~Mat();

		int c;
		double resizeFactorWidth = 1;
		double resizeFactorHeight = 1;
		double frameInitialWidth = capture.get(CV_CAP_PROP_FRAME_WIDTH);

		double frameInitialHeight = capture.get(CV_CAP_PROP_FRAME_HEIGHT);

		
		getCurrentHourMinSec(hourStartKeepAlive, minStartKeepAlive, secStartKeepAlive);
		getCurrentHourMinFromConf(hourStartSendLogs, minStartSendLogs);
		
		while (true) {
			A = high_resolution_clock::now();


			/////////////////////////////////////////

			//////////////////////////////////////
			if (timeToSendLogFiles())
			{
				sendLogFiles();
			}
			//auto milli = duration_cast<milliseconds>(t11.time_since_epoch()).count();
			//std::cout <<"pre: "<< milli << '\n';
			capture.read(frame);
			
			if (current_newVersion == true) {
				setnewVersion(false);
				


				if ((frameInitialHeight >= current_USER_CAP_PROP_FRAME_HEIGHT) && (current_USER_CAP_PROP_FRAME_HEIGHT > 0)){

					resizeFactorHeight = current_USER_CAP_PROP_FRAME_HEIGHT / frameInitialHeight;
				}

				if ((frameInitialWidth >= current_USER_CAP_PROP_FRAME_WIDTH) && (current_USER_CAP_PROP_FRAME_WIDTH > 0)) {

					resizeFactorWidth = current_USER_CAP_PROP_FRAME_WIDTH / frameInitialWidth;
				}
				

			}
			resize(frame, outputframe, cv::Size(), resizeFactorWidth, resizeFactorHeight, CV_INTER_LINEAR);
			frame = outputframe;
			//-- 3. Apply the classifier to the frame
			if (!frame.empty()) {
				
				detectAndDisplay(frame);
				
			}
			else {

				printf(" --(!) No captured frame -- Break!"); break;
			}

			
			
			c = cv::waitKey(1);
			
			if ((char)c == 'c') { break; }	
			D = high_resolution_clock::now();
			auto durationBA = std::chrono::duration_cast<std::chrono::milliseconds>(B - A).count();
			std::cout << "//////////////////////////////////" << std::endl << " pre:" << durationBA << std::endl;
			auto durationtwoCB = std::chrono::duration_cast<std::chrono::milliseconds>(C - B).count();
			std::cout << "proc:" << durationtwoCB << std::endl;
			auto durationthree = std::chrono::duration_cast<std::chrono::milliseconds>(D - C).count();
			std::cout << "post:" << durationthree << std::endl << "//////////////////////////////////" << std::endl;
			std::cout << "version:" << current_version<< std::endl;
			//sum += duration;
			//ii++;
		//	std::cout << "M: " << duration - durationtwo << " ";
		//	auto milli = duration_cast<milliseconds>(t21.time_since_epoch()).count();
			//std::cout << "post: " << milli << '\n';
		}
		
	}
//}

void ConfigureParameters() {


	FILE *pFile = fopen("Configuration.txt", "r");
	std::map<int, int> finalListRezolution;
	std::ofstream out;

	if (pFile != NULL) { //file exist

		fclose(pFile);

		std::ifstream in("Configuration.txt");
		std::string line;
		if (in.is_open()) {

			while (getline(in, line))
			{

				std::istringstream iss(line); // access line as a stream
				int key;
				int val;

				iss >> key >> val; // no need to read further
				switch (key)
				{
				case 0:
					current_version = val;
					break;
				case 1:
					//current_WINDOWNAME = ssval;
					break;
				case 2:
					current_USER_CAP_PROP_FRAME_WIDTH = val;
					break;
				case 3:
					current_USER_CAP_PROP_FRAME_HEIGHT = val;
					break;
				case 4:
					current_SCALEFACTOR = (double)(((double)val / 100) + 1);
					break;
				case 5:
					current_MINIMUMNEIGBOURS = val;
					break;
				case 6:
					current_minXSizeSE = val;
					break;
				case 7:
					current_minYSizeSE = val;
					break;
				case 8:
					current_DISTANCEFACETOBEDETECTEDX = val;
					break;
				case 9:
					current_DISTANCEFACETOBEDETECTEDY = val;
					break;
				case 10:
					current_MINIMUMFACEAPPARITION = val;
					break;
				case 11:
					//current_BACKOFFICEURL = ssval;
					break;
				case 12:
					current_FRAMESUNTILDELETEDETECTION = val;
					break;
				case 13:
					current_drawDetection = val;

					break;
				case 14:
					current_displayVideo = val;

					break;
				case 15:
					current_Notifybackoffice = val;

					break;
				case 16:
					current_rezolutionSent = val;

					break;
				case 17:
					current_minutesToSendLogFile = val;
					break;
				case 18:
					current_lastTimeMinutesSentLogFile = val;
				case 20:
					current_secToSendKeepAlive = val;
					break;
				default:
					break;
				}

			}

			in.close();
			setnewVersion(true);
		}
	}
	else {


		out.open("Configuration.txt");

		out << "0  " << default_VERSION_OURAPPLICATION << " .version " << std::endl;
		//out << "1  " << WINDOWNAME << " .windowName " << std::endl;
		out << "2  " << default_USER_CAP_PROP_FRAME_WIDTH << " .cameraResolutionWidth " << " px" << std::endl;
		out << "3  " << default_USER_CAP_PROP_FRAME_HEIGHT << " .cameraResolutionHeight " << " px" << std::endl;
		out << "4  " << ((default_SCALEFACTOR - 1) * 100) << " .scaleFactor " << " %   " << "Recommended: 1-->100 " << std::endl;
		out << "5  " << default_MINIMUMNEIGBOURS << " .minimumNeighbours " << "   Recommended: 0-->10" << std::endl;
		out << "6  " << default_minXSizeSE << " .minimumFacesizeX " << " px" << std::endl;
		out << "7  " << default_minYSizeSE << " .minimumFacesizeY " << " px" << std::endl;
		out << "8  " << default_DISTANCEFACETOBEDETECTEDX << " .movementOffsetX " << " px" << std::endl;
		out << "9  " << default_DISTANCEFACETOBEDETECTEDY << " .movementOffsetY " << " px" << std::endl;
		out << "10  " << default_MINIMUMFACEAPPARITION << " .detectionMinimumThreshold " << " frames" << std::endl;
		//out << "11  " << default_BACKOFFICEURL << " .backOfficeUrl " << std::endl;
		out << "12  " << default_FRAMESUNTILDELETEDETECTION << " .framesUntilDeleteDetection " << " frames" << std::endl;
		out << "13  " << "1" << " .drawDetection " << std::endl;
		out << "14  " << "1" << " .displayVideo " << std::endl;
		out << "15  " << "1" << " .Notifybackoffice " << std::endl;
		out << "16  " << "0" << " .rezolutionSent " << std::endl;
		out << "17  " << current_minutesToSendLogFile << " .minutesToSendLogFile " << std::endl;
		out << "18  " << current_lastTimeMinutesSentLogFile << " .current_lastTimeMinutesSentLogFile " << std::endl;
		out << "20  " << current_secToSendKeepAlive << " .secToSendKeepAlive " << std::endl;
		std::cout << "configuration file written... " << std::endl;

		out.close();

		current_version = default_VERSION_OURAPPLICATION;
		//current_WINDOWNAME = ssval;
		current_USER_CAP_PROP_FRAME_WIDTH = default_USER_CAP_PROP_FRAME_WIDTH;
		current_USER_CAP_PROP_FRAME_HEIGHT = default_USER_CAP_PROP_FRAME_HEIGHT;
		current_SCALEFACTOR = default_SCALEFACTOR;
		current_MINIMUMNEIGBOURS = default_MINIMUMNEIGBOURS;
		current_minXSizeSE = default_minXSizeSE;
		current_minYSizeSE = default_minYSizeSE;
		current_DISTANCEFACETOBEDETECTEDX = default_DISTANCEFACETOBEDETECTEDX;
		current_DISTANCEFACETOBEDETECTEDY = default_DISTANCEFACETOBEDETECTEDY;
		current_MINIMUMFACEAPPARITION = default_MINIMUMFACEAPPARITION;
		//current_BACKOFFICEURL = ssval;
		current_FRAMESUNTILDELETEDETECTION = default_FRAMESUNTILDELETEDETECTION;
		current_drawDetection = 1;
		current_displayVideo = 1;
		current_Notifybackoffice = 1;
		current_rezolutionSent = 0;
		current_secToSendKeepAlive = 1;
		current_minutesToSendLogFile = 400;
		setnewVersion(true);
	}

	if (current_rezolutionSent != 1){

		cv::VideoCapture capture(sourceVideo);
		getSupportedResolution(capture, finalListRezolution);
		capture.~VideoCapture();
		postRegister(true, finalListRezolution);
		current_rezolutionSent = 1;
		saveConfiguration();

	}
	else {

		postRegister(false, finalListRezolution);
	}

	/*
	std::ifstream in("Configuration.txt");
	int key, val;
	std::string line;
	if (in.is_open()) {

		while (getline(in, line)) {

			std::istringstream iss(line); // access line as a stream
			int key;
			int val;

			iss >> key >> val;
			switch (key)
			{
			case 16:
				if (val != 1) {

					//postRegister(true, finalListRezolution);
					//setam in fisier
					break;
			default:
				break;
				
				}
			}
		}
		in.close();
	}
	else {

				postRegister(false, finalListRezolution);
				in.close();
		}
			*/
	finalListRezolution.clear();
	
}


bool timeToSendLogFiles() {
	int hour = 0, min = 0;
	getCurrentHourMin(hour, min);

	int minutesFromLastLogsSent = 0;
	minutesFromLastLogsSent = (hour - hourStartSendLogs) * 60 + (min - minStartSendLogs);

	if (minutesFromLastLogsSent > current_minutesToSendLogFile) {
	current_lastTimeMinutesSentLogFile = hour * 100 + min;
	getCurrentHourMinFromConf(hourStartSendLogs, minStartSendLogs);
	 void saveConfiguration();
	return true;
	}
	else
		return false;
	
	/*if ((todayLogFilesWasSent == false) && (hour == current_HOURTOSENDLOGFILES) && (min == current_MINTOSENDLOGFILES)) {

		todayLogFilesWasSent = true;
		return true;
	}
	else
	{
		if (hour == current_HOURTOSENDLOGFILESRESET) {
			todayLogFilesWasSent = false;
		}
		return false;
	}*/
}

inline void getCurrentHourMin(int &hour, int &min) {
	time_t currentTime;
	struct tm *localTime;

	time(&currentTime);
	localTime = localtime(&currentTime);
	hour = localTime->tm_hour;
	min = localTime->tm_min;
}

inline void getCurrentHourMinSec(int &hour, int &min, int &sec) {
	time_t currentTime;
	struct tm *localTime;

	time(&currentTime);
	localTime = localtime(&currentTime);
	hour = localTime->tm_hour;
	min = localTime->tm_min;
	sec = localTime->tm_sec;
}

inline void getCurrentHourMinFromConf(int &hour, int &min) {

	min = current_lastTimeMinutesSentLogFile % 100;
	hour = current_lastTimeMinutesSentLogFile / 100;
}

void getCurrentDay(int &day, int &hour, int &min) {

	time_t currentTime;
	struct tm *localTime;

	time(&currentTime);
	localTime = localtime(&currentTime);
	day = localTime->tm_mday;
	hour = localTime->tm_hour;
	min = localTime->tm_min;
}

void get_all_files_names_within_folder(std::string folder, std::vector<std::string> &names) {

	int day = 0, hour = 0, min = 0;
	getCurrentDay(day, hour, min);
	char search_path[200];
	sprintf(search_path, "%s/*.*", folder.c_str());
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(search_path, &fd);
	std::string stringvar;
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// read all (real) files in current folder
			// , delete '!' read other 2 default folder . and ..
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {

			//	FILETIME CreationTimeFile = fd.ftCreationTime;
			//	LPSYSTEMTIME normalCreationTimeFile;
			//	if (!FileTimeToSystemTime(&CreationTimeFile, normalCreationTimeFile)) {

			//		std::cout << "------error at getting file time creation-----------";	
			//	}
			//	std::cout << currentday;

//				if (normalCreationTimeFile->wDay == (currentday-1)) {
					//char ch[260];
				//	char DefChar = ' ';
				//	WideCharToMultiByte(CP_ACP, 0, fd.cFileName, -1, ch, 260, &DefChar, NULL);
					
					//A std:string  using the char* constructor.
					//std::string stringtoAdd(fd.cFileName);
					//stringvar = stringtoAdd;
					//std::cout << "sad" << stringvar;
					std::stringstream myStreamString;
					myStreamString << fd.cFileName;
					std::string myString = myStreamString.str();
					myString = myString + '\0';
					names.push_back(myString);
			//	}
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
}

void sendLogFiles() {

	std::vector<std::string> filesVector;
	std::vector<std::string> bigStringFromFiles;
	get_all_files_names_within_folder(LogFilesPath, filesVector);
	std::string searchString = "g C:";
	std::string bigString;
	for (std::vector<std::string>::iterator it = filesVector.begin(); it != filesVector.end(); ++it) {

		std::string nameFile = "C:\\Program Files\\Ayuda Media Systems\\Splash Player\\log\\" + *it;
		std::ifstream ifs(nameFile);

		if (ifs.is_open()) {

			std::string line;
			while (getline(ifs,line)) {

				std::size_t found = line.find(searchString);
				if (found != std::string::npos) {
					//#########################PRELUCRARE LINIE
					
					std::string strStart = line.substr(0, 23); 
					std::size_t posLast = line.rfind('\\');
					std::string strFinish = line.substr(posLast); 
					//##########################
					std::string newLine = strStart + strFinish;
					bigString.append("#" + newLine);

				}
			}
		}
		ifs.close();
		if (!bigString.empty())
			bigStringFromFiles.push_back(bigString);
		bigString.clear();
	}


	//acum avem vectorul de stringuri chiar frumos aranjat si doar esentialul.
	for (std::vector<std::string>::iterator it = bigStringFromFiles.begin(); it != bigStringFromFiles.end(); ++it) {

		//aici vom face posturile
		std::thread(postFileLog, *it).detach();
	}
}



/*cv::VideoCapture openVideo() {

cv::VideoCapture capture = sourceVideo(0);
std::vector<int> listCameras;
//de modificat incercare gasire camera
getListCameras(listCameras);


//de gasite rezolutii disponibile
/*<height,width>*/
/*listCameras.clear();


#ifdef ACTIVATE_MANUAL_CAMERA_CONFIGURATION

if (USER_CAP_PROP_POS_MSEC != 0) {
//erorare de setare
capture.set(cv::CAP_PROP_POS_MSEC, USER_CAP_PROP_POS_MSEC);
printf("// Current position of the video file in milliseconds.\n");
}

if (USER_CAP_PROP_POS_FRAMES != 0) {

capture.set(cv::CAP_PROP_POS_FRAMES, USER_CAP_PROP_POS_FRAMES);
printf("//  0 - based index of the frame to be decoded / captured next.\n");
}

if (USER_CAP_PROP_POS_AVI_RATIO != 0) {

capture.set(cv::CAP_PROP_POS_AVI_RATIO, USER_CAP_PROP_POS_AVI_RATIO);
printf("//Relative position of the video file : 0 - start of the film, 1 - end of the film.\n");
}

if (USER_CAP_PROP_FRAME_WIDTH != 0) {

capture.set(cv::CAP_PROP_FRAME_WIDTH, USER_CAP_PROP_FRAME_WIDTH);
printf("//Width of the frames in the video stream\n.");
}

if (USER_CAP_PROP_FRAME_HEIGHT != 0) {

capture.set(cv::CAP_PROP_FRAME_HEIGHT, USER_CAP_PROP_FRAME_HEIGHT);
printf("//Height of the frames in the video stream.\n");
}

if (USER_CAP_PROP_FPS != 0) {

capture.set(cv::CAP_PROP_FPS, USER_CAP_PROP_FPS);
printf("//Frame rate.\n");
}

if (USER_CAP_PROP_FOURCC != 0) {

capture.set(cv::CAP_PROP_FOURCC, USER_CAP_PROP_FOURCC);
printf("//4 - character code of codec.\n");
}

if (USER_CAP_PROP_FRAME_COUNT != 0) {

capture.set(cv::CAP_PROP_FRAME_COUNT, USER_CAP_PROP_FRAME_COUNT);
printf("//Number of frames in the video file.\n");
}

if (USER_CAP_PROP_FORMAT != 0) {

capture.set(cv::CAP_PROP_FORMAT, USER_CAP_PROP_FORMAT);
printf("//Format of the Mat objects returned by retrieve() .\n");
}

if (USER_CAP_PROP_MODE != 0) {

capture.set(cv::CAP_PROP_MODE, USER_CAP_PROP_MODE);
printf("//Backend - specific value indicating the current capture mode.\n");
}
if (USER_CAP_PROP_BRIGHTNESS != 0){

capture.set(cv::CAP_PROP_BRIGHTNESS, USER_CAP_PROP_BRIGHTNESS);
printf("//Brightness of the image(only for cameras).\n");
}

if (USER_CAP_PROP_CONTRAST != 0) {

capture.set(cv::CAP_PROP_CONTRAST, USER_CAP_PROP_CONTRAST);
printf("//Contrast of the image(only for cameras).\n");
}

if (USER_CAP_PROP_SATURATION != 0) {

capture.set(cv::CAP_PROP_SATURATION, USER_CAP_PROP_SATURATION);
printf("// Saturation of the image(only for cameras).\n");
}

if (USER_CAP_PROP_HUE != 0) {

capture.set(cv::CAP_PROP_HUE, USER_CAP_PROP_HUE);
printf("// Hue of the image(only for cameras).\n");
}

if (USER_CAP_PROP_GAIN != 0) {

capture.set(cv::CAP_PROP_GAIN, USER_CAP_PROP_GAIN);
printf("//Gain of the image(only for cameras).\n");
}

if (USER_CAP_PROP_EXPOSURE != 0) {

capture.set(cv::CAP_PROP_EXPOSURE, USER_CAP_PROP_EXPOSURE);
printf("// Exposure(only for cameras).\n");
}

if (USER_CAP_PROP_CONVERT_RGB != 0) {

capture.set(cv::CAP_PROP_CONVERT_RGB, USER_CAP_PROP_CONVERT_RGB);
printf("//Boolean flags indicating whether images should be convertedto RGB.\n");
}

if (USER_CAP_PROP_RECTIFICATION != 0) {

capture.set(cv::CAP_PROP_RECTIFICATION, USER_CAP_PROP_RECTIFICATION);
printf("//Rectification flag for stereo cameras(note: only supportedby DC1394 v 2.x backend currently)\n");
}

#endif // ACTIVATE_MANUAL_CAMERA_CONFIGURATION

if (!capture.isOpened()) {
std::cerr << "Failed to open the video device, video file or image sequence! \n";
/*DE ELIMINAT*/
/*return capture;
}
//assert(capture.isOpened());
return capture;
}*/