#define CURL_STATICLIB


#include "opencvheader.h"

#include <sstream>
#include <fstream>
#include <string>
#include <iostream>

int RegisterApplication();
void GenerateUID(char* &);
void LoadRegisterInfo();
int postRegister(bool, std::map<int, int> &);
int postKeepAlive();
int postImpression(long long, time_t, int, int, int, int, cv::Mat);
int postFileLog(std::string);
void saveConfiguration();
void setnewVersion(bool);
bool getCurrentNewVersion();
void writeDetectionToDisk(std::string);
void resendOldDetection();
bool ResendDetection(std::string);