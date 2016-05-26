#ifndef openCVHEADERS

#define openCVHEADERS

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <map>
#include <iostream>
#include <stdio.h>
#include <thread> 
#include <Windows.h>

struct RegisterInfo
{
	std::string UID;
	std::string MachineIPv4String;
	std::string Version;

};

extern RegisterInfo RegisterInfoNow;
#endif // openCVHEADERS


