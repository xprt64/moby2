#include "opencvheader.h"

#ifndef MYPARAMS

#define MYPARAMS

/** For camera */
#define sourceVideo CV_CAP_ANY

/** For video input */
//#define sourceVideo "vid7.mp4"

/** Camera Configuration */
/** NULL = default. Replace NULL with desired value (double). WHEN CONFIGURE A PARAMETER, ACTIVATE_MANUAL_CAMERA_CONFIGURATION  */
///default parameters

#define ACTIVATE_MANUAL_CAMERA_CONFIGURATION true

static const double default_USER_CAP_PROP_POS_MSEC			= NULL;	// Current position of the video file in milliseconds.\n");
static const double default_USER_CAP_PROP_POS_FRAMES = NULL;	//  0 - based index of the frame to be decoded / captured next.\n");
static const double default_USER_CAP_PROP_POS_AVI_RATIO = NULL;	//Relative position of the video file : 0 - start of the film, 1 - end of the film.\n");
/*Note that even if you want to resize with any x and y value that will not happen, because it only depend by hardware configuration. It will be
selected the sizes which are closer*/
					
static const double default_USER_CAP_PROP_FPS				= NULL;	//Frame rate.\n");
static const double default_USER_CAP_PROP_FOURCC			= NULL;	//4 - character code of codec.\n");
static const double default_USER_CAP_PROP_FRAME_COUNT		= NULL;	//Number of frames in the video file.\n");
static const double default_USER_CAP_PROP_FORMAT			= NULL;	//Format of the Mat objects returned by retrieve() .\n");
static const double default_USER_CAP_PROP_MODE				= NULL;	//Backend - specific value indicating the current capture mode.\n");
static const double default_USER_CAP_PROP_BRIGHTNESS		= NULL;	//Brightness of the image(only for cameras).\n");
static const double default_USER_CAP_PROP_CONTRAST			= NULL;	//Contrast of the image(only for cameras).\n");
static const double default_USER_CAP_PROP_SATURATION		= NULL;	// Saturation of the image(only for cameras).\n");
static const double default_USER_CAP_PROP_HUE				= NULL;	// Hue of the image(only for cameras).\n");
static const double default_USER_CAP_PROP_GAIN				= NULL;	//Gain of the image(only for cameras).\n");
static const double default_USER_CAP_PROP_EXPOSURE			= NULL;	// Exposure(only for cameras).\n");
static const double default_USER_CAP_PROP_CONVERT_RGB		= NULL;	//Boolean flags indicating whether images should be convertedto RGB.\n");
static const double default_USER_CAP_PROP_RECTIFICATION	= NULL;	//Rectification flag for stereo cameras(note: only supportedby DC1394 v 2.x backend currently)\n");

static const std::string default_TRAINER_OUR_PROGRAM = "haarcascade_frontalface_alt.xml"; //trainer for our program

/** In TRAINER_OUR_PROGRAM there is a trainer. He describe a model. The model has a dimensions, in our case (20,20).
Each frame is traversed by model. After each iteration the frame is traversed again by model. But this time
the model is resized. How much? Scale factor decide it.
Affect performance. SCALEFACETOR is proportional with quality of detection, but proportional with performance*/
static const double default_SCALEFACTOR = 1.1;

/** After the iterations on one frame are completely done, one face will be detected multiple times.
In order to overcame this situation, the alghoritm can detect if a face was taken multiple times by setting MINIMUMNEIGHBOURS
which set the minimum number of possible faces so the face be detected.
This parameters affect false positive and false negative.*/
static const int default_MINIMUMNEIGBOURS = 2;

/** This affect directly the distance from which the camera detect faces!
Greater distance means low performances.*/

static const int default_minXSizeSE = 20;
static const int default_minYSizeSE = 20;

/** Do not change it unless you want to detect faces few meters away from camera*/
static const int default_maxXSizeSE = NULL;
static const int default_maxYSizeSE = NULL;

/** This parameter decide how much the face detected will be considered anterior face
Set the maximum distance of face detected to anterior face detected, such that the facedetected
to be considered as anteterior facedetected */
static const int default_DISTANCEFACETOBEDETECTEDX = 40;
static const int default_DISTANCEFACETOBEDETECTEDY = 40;

static const int default_MINIMUMFACEAPPARITION = 10;

static const std::string default_BACKOFFICEURL = "http://tru-reach.com";

static const std::string default_WINDOWNAME = "Capture - Face detection";

static const int default_FRAMESUNTILDELETEDETECTION = 10;

static const int default_VERSION_OURAPPLICATION = 0;
#endif 




