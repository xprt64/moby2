#include "NetworkModule.h"
#include <stdio.h>
#include <curl/curl.h>
#include "opencvheader.h"
#include <windows.h>
#include "params.h"
#include <thread>
#include <mutex>

std::mutex mu;
std::mutex mutexFileDetection;
std::mutex mutexResendDetection;

#pragma comment(lib, "rpcrt4.lib") 
#pragma comment(lib, "Ws2_32.lib")

extern bool current_newVersion;

void setnewVersion(bool val) {

	mu.lock();
	bool val2 = val;
	current_newVersion = val2;
	mu.unlock();
}

extern std::string current_BACKOFFICEURL;
extern int current_version;
extern int current_USER_CAP_PROP_FRAME_WIDTH;
extern int current_USER_CAP_PROP_FRAME_HEIGHT;
extern double current_SCALEFACTOR;
extern int current_MINIMUMNEIGBOURS;
extern int current_minXSizeSE;
extern int current_minYSizeSE;
extern int current_DISTANCEFACETOBEDETECTEDX;
extern int current_DISTANCEFACETOBEDETECTEDY;
extern int current_MINIMUMFACEAPPARITION;
extern int current_FRAMESUNTILDELETEDETECTION;
extern int current_drawDetection;
extern int current_displayVideo;
extern int current_Notifybackoffice;
extern int current_rezolutionSent;
extern bool current_newVersion;
extern int current_HOURTOSENDLOGFILESRESET;
extern int current_HOURTOSENDLOGFILES;
extern int current_MINTOSENDLOGFILES;
extern unsigned __int32 current_secToSendKeepAlive;
extern int current_minutesToSendLogFile;
extern int current_lastTimeMinutesSentLogFile;

RegisterInfo RegisterInfoNow;
struct IPv4
{
	int b1, b2, b3, b4;
};

void GenerateUID(char* &str) {
 // UuidCreate - Minimum supported OS Win 2000
		UUID uuid;
		UuidCreate(&uuid);
		//char *str;
		UuidToStringA(&uuid, (RPC_CSTR*)&str);
		std::cout << str << std::endl;
		//strcpy_s(stringreturn,(strlen(str) +1) , str);
}

bool getMyIP(IPv4 & myIP)
{
		char szBuffer[1024];

#ifdef WIN32
		WSADATA wsaData;
		WORD wVersionRequested = MAKEWORD(2, 0);
		if (::WSAStartup(wVersionRequested, &wsaData) != 0)
			return false;
#endif


		if (gethostname(szBuffer, sizeof(szBuffer)) == SOCKET_ERROR)
		{
#ifdef WIN32
			WSACleanup();
#endif
			return false;
		}

		struct hostent *host = gethostbyname(szBuffer);
		if (host == NULL)
		{
#ifdef WIN32
			WSACleanup();
#endif
			return false;
		}

		//Obtain the computer's IP
		myIP.b1 = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b1;
		myIP.b2 = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b2;
		myIP.b3 = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b3;
		myIP.b4 = ((struct in_addr *)(host->h_addr))->S_un.S_un_b.s_b4;

#ifdef WIN32
		WSACleanup();
#endif
		return true;
}

std::string GetVersionPlatform(){

	unsigned long dwVersion = 0;
	unsigned long dwMajorVersion = 0;
	unsigned long dwMinorVersion = 0;
	unsigned long dwBuild = 0;
	std::string numberMajVersion;
	std::string numberMinVersion;
	std::stringstream strstream;

	dwVersion = GetVersion();

	// Get the Windows version.

	dwMajorVersion = (unsigned long)(LOBYTE(LOWORD(dwVersion)));
	dwMinorVersion = (unsigned long)(HIBYTE(LOWORD(dwVersion)));

	// Get the build number.

	if (dwVersion < 0x80000000)
		dwBuild = (DWORD)(HIWORD(dwVersion));

	printf("Version is %d.%d (%d)\n",
		dwMajorVersion,
		dwMinorVersion,
		dwBuild);

	strstream << dwMajorVersion;
	strstream >> numberMajVersion;
	strstream.clear();
	strstream << dwMinorVersion;
	strstream >> numberMinVersion;
	return (numberMajVersion + "." + numberMinVersion);
	
}

void LoadRegisterInfo() {

	std::ifstream in("UID.txt");
	std::string key, val;
	while (in >> key >> val) {
		if (key == "UID")
			RegisterInfoNow.UID = val;
	}
	in.close();
}

int RegisterApplication(){

	FILE *pFile = fopen("UID.txt", "r");
	char *UID;
	
	std::ofstream out;
	std::string preparedPost;

	if (pFile != NULL) {

		fclose(pFile); 
		LoadRegisterInfo();
	}
	else {

		GenerateUID(UID);
		int size = strlen(UID);
		preparedPost.assign(UID, size);
		RegisterInfoNow.UID = preparedPost;
		out.open("UID.txt");
		out << "UID " << RegisterInfoNow.UID << std::endl;
		out.close();
	}

	return 0;
}

struct MemoryStruct {
	char *memory;
	size_t size;
};

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory == NULL) {
		/* out of memory! */
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

int postRegister(bool sendCameraRezolutions, std::map<int, int> &finalListRezolution) {

	IPv4 MachineIPv4;
	std::string preparedPost;
	RegisterInfoNow.Version = GetVersionPlatform();
	getMyIP(MachineIPv4);
	RegisterInfoNow.MachineIPv4String = std::to_string(MachineIPv4.b1) + "." + std::to_string(MachineIPv4.b2) + "." + std::to_string(MachineIPv4.b3) + "." + std::to_string(MachineIPv4.b4);

	preparedPost = "uuid=" + RegisterInfoNow.UID + "&name=Intel Compute Stick" + "&platform=" + RegisterInfoNow.Version + "&ip=" + RegisterInfoNow.MachineIPv4String;

	if (sendCameraRezolutions == true) {

		std::string rezolutions;
		for (std::map<int, int>::iterator it = finalListRezolution.begin(); it != finalListRezolution.end(); ++it) {

			if (it != finalListRezolution.begin())
			{
				rezolutions += ",";
			}
			rezolutions += std::to_string(it->second) + "x" + std::to_string(it->first);
		}

		preparedPost += "&resolutions=" + rezolutions;
	}

	std::string restURL = "/api/registerDevice/";
	std::string completeURL = current_BACKOFFICEURL + restURL;
	CURL *curl;
	CURLcode res;
	struct MemoryStruct chunk;

	chunk.memory = (char *)malloc(1);  /* will be grown as needed by the realloc above */
	chunk.size = 0;    /* no data at this point */
	curl_global_init(CURL_GLOBAL_DEFAULT);

	//RpcStringFreeA((RPC_CSTR*)&preparedPost);
	curl = curl_easy_init();
	if (curl) {

		curl_easy_setopt(curl, CURLOPT_URL, completeURL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

		/* we pass our 'chunk' struct to the callback function */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, preparedPost);
#ifdef SKIP_PEER_VERIFICATION
		/*
		* If you want to connect to a site who isn't using a certificate that is
		* signed by one of the certs in the CA bundle you have, you can skip the
		* verification of the server's certificate. This makes the connection
		* A LOT LESS SECURE.
		*
		* If you have a CA cert for the server stored someplace else than in the
		* default bundle, then the CURLOPT_CAPATH option might come handy for
		* you.
		*/
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

#ifdef SKIP_HOSTNAME_VERIFICATION
		/*
		* If the site you're connecting to uses a different host name that what
		* they have mentioned in their server certificate's commonName (or
		* subjectAltName) fields, libcurl will refuse to connect. You can skip
		* this check, but this will make the connection less secure.
		*/
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif
		//0:1,2:144,3:176,4:10,5:2,6:20,7:20,8:40,9:40,10:10,12:60,13:0,14:0,15:1,status:ok
		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		int key[50] = { 0 };
		int value[50] = { 0 };
		int index = 1;
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
		}
		else
		{
			printf("%lu bytes retrieved\n", (long)chunk.size);
			char *sPtr = nullptr;
			char *newTemp = nullptr;
			//sPtr = strtok(chunk.memory, ",");
			std::string newstring(chunk.memory);
			//newstring = "0:1, 2 : 144, 3 : 176, 4 : 10, 5 : 2, 6 : 20, 7 : 20, 8 : 40, 9 : 40, 10 : 10, 12 : 60, 13 : 0, 14 : 0, 15 : 1, status : ok";
			size_t pos = 0;
			std::string delimiter = ",";
			std::string token;

			while ((pos = newstring.find(delimiter)) != std::string::npos) {
				token = newstring.substr(0, pos);
				std::cout << token << std::endl;
				{
					size_t pos1 = 0;
					std::string delimiter1 = ":";
					std::string token1;

					while ((pos1 = token.find(delimiter1)) != std::string::npos) {

						token1 = token.substr(0, pos1);
						std::cout << token1 << std::endl;
						key[index] = std::stoi(token1);
						token.erase(0, pos1 + delimiter1.length());
						value[index] = std::stoi(token);
						std::cout << token << std::endl;
						++index;
					}
				}

				newstring.erase(0, pos + delimiter.length());
			}

			std::cout << newstring << std::endl;


			std::cout << std::endl;

			if (current_version < value[1]) {
				std::cout << std::endl << "A venit o noua versiune: " << value[1];
				current_version = value[1];

				for (int j = 1; j <= index; j++) {

					switch (key[j]) {
					case 0:
						break;
					case 1:
						//current_WINDOWNAME = ssval;
						break;
					case 2:
						current_USER_CAP_PROP_FRAME_WIDTH = value[j];
						break;
					case 3:
						current_USER_CAP_PROP_FRAME_HEIGHT = value[j];
						break;
					case 4:
						current_SCALEFACTOR = ((double)value[j] / 100 + 1);
						break;
					case 5:
						current_MINIMUMNEIGBOURS = value[j];
						break;
					case 6:
						current_minXSizeSE = value[j];
						break;
					case 7:
						current_minYSizeSE = value[j];
						break;
					case 8:
						current_DISTANCEFACETOBEDETECTEDX = value[j];
						break;
					case 9:
						current_DISTANCEFACETOBEDETECTEDY = value[j];
						break;
					case 10:
						current_MINIMUMFACEAPPARITION = value[j];
						break;
					case 11:
						//current_BACKOFFICEURL = ssval;
						break;
					case 12:
						current_FRAMESUNTILDELETEDETECTION = value[j];
						break;
					case 13:
						current_drawDetection = value[j];
						break;
					case 14:
						current_displayVideo = value[j];
						break;
					case 15:
						current_Notifybackoffice = value[j];
						break;
					default:
						break;
					}
				}

				//write on disk
				saveConfiguration();

				setnewVersion(true);
			}

			/* always cleanup */
			curl_easy_cleanup(curl);
		}
		free(chunk.memory);
		curl_global_cleanup();

		return 0;
		//o trimit post si astept confirmare
	}
}

void saveConfiguration() {

	std::ofstream out;
	out.open("Configuration.txt");
	if (out.good()) {

		out << "0  " << current_version << " .version " << std::endl;
		//out << "1  " << WINDOWNAME << " .windowName " << std::endl;
		out << "2  " << current_USER_CAP_PROP_FRAME_WIDTH << " .cameraResolutionWidth " << " px" << std::endl;
		out << "3  " << current_USER_CAP_PROP_FRAME_HEIGHT << " .cameraResolutionHeight " << " px" << std::endl;
		out << "4  " << ((current_SCALEFACTOR - 1) * 100) << " .scaleFactor " << " %   " << "Recommended: 1-->100 " << std::endl;
		out << "5  " << current_MINIMUMNEIGBOURS << " .minimumNeighbours " << "   Recommended: 0-->10" << std::endl;
		out << "6  " << current_minXSizeSE << " .minimumFacesizeX " << " px" << std::endl;
		out << "7  " << current_minYSizeSE << " .minimumFacesizeY " << " px" << std::endl;
		out << "8  " << current_DISTANCEFACETOBEDETECTEDX << " .movementOffsetX " << " px" << std::endl;
		out << "9  " << current_DISTANCEFACETOBEDETECTEDY << " .movementOffsetY " << " px" << std::endl;
		out << "10  " << current_MINIMUMFACEAPPARITION << " .detectionMinimumThreshold " << " frames" << std::endl;
		//out << "11  " << BACKOFFICEURL << " .backOfficeUrl " << std::endl;
		out << "12  " << current_FRAMESUNTILDELETEDETECTION << " .framesUntilDeleteDetection " << " frames" << std::endl;
		out << "13  " << current_drawDetection << " .drawDetection " << std::endl;
		out << "14  " << current_displayVideo << " .displayVideo " << std::endl;
		out << "15  " << current_Notifybackoffice << " .Notifybackoffice " << std::endl;
		out << "16  " << current_rezolutionSent << " .rezolutionSent " << std::endl;
		out << "17  " << current_minutesToSendLogFile << " .current_minutesToSendLogFile " << std::endl;
		out << "18  " << current_lastTimeMinutesSentLogFile << " .current_lastTimeMinutesSentLogFile " << std::endl;
		out << "20  " << current_secToSendKeepAlive << " .current_secToSendKeepAlive " << std::endl;

		std::cout << "configuration file written... " << std::endl;
	}
	out.close();
}

int postImpression(long long duration, time_t timestamp, int xToSend, int yToSend, int heightToSend, int WidthToSend, cv::Mat croppedFace) {
	
	int b = 0;
	int g = 0;
	int r = 0;
	
	std::string imagestr;
	for (int j = 0; j < croppedFace.rows; j++){
		for (int i = 0; i < croppedFace.cols; i++){
			cv::Vec3b pixel = croppedFace.at<cv::Vec3b>(j, i);

			b = pixel[0];
			g = pixel[1];
			r = pixel[2];
			std::stringstream ss;
			ss << b;
			std::string strb = ss.str();

			std::stringstream ss1;
			ss1 << g;
			std::string strg = ss1.str();

			std::stringstream ss2;
			ss2 << r;
			std::string strr = ss2.str();
		
			imagestr = imagestr + "(" + strb + "," + strg + "," + strr + ");";
			// end of pixel processing ----------------
			// end of line
		}

	}

	std::string preparedPost;
	if (current_Notifybackoffice == 1) {

		std::string completeURL;
		std::stringstream sstringWrapperTimestamp;
		sstringWrapperTimestamp << timestamp;
		std::string stringWrapper = sstringWrapperTimestamp.str();
		preparedPost = "uuid=" + RegisterInfoNow.UID + "&timestamp=" + stringWrapper + "&duration=" + std::to_string(duration)+
			"&x=" + std::to_string(xToSend) + "&y=" + std::to_string(yToSend) + "&height=" + std::to_string(heightToSend) + "&width=" + std::to_string(WidthToSend) + "&faceString=" + imagestr;
		std::string restURL = "/api/saveImpression/";
		completeURL = current_BACKOFFICEURL + restURL;
		CURL *curl;
		CURLcode res;

		curl_global_init(CURL_GLOBAL_DEFAULT);

		//RpcStringFreeA((RPC_CSTR*)&preparedPost);
		curl = curl_easy_init();
		if (curl) {
			curl_easy_setopt(curl, CURLOPT_URL, completeURL);
			curl_easy_setopt(curl, CURLOPT_POST, 1);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, preparedPost);
#ifdef SKIP_PEER_VERIFICATION
			/*
			* If you want to connect to a site who isn't using a certificate that is
			* signed by one of the certs in the CA bundle you have, you can skip the
			* verification of the server's certificate. This makes the connection
			* A LOT LESS SECURE.
			*
			* If you have a CA cert for the server stored someplace else than in the
			* default bundle, then the CURLOPT_CAPATH option might come handy for
			* you.
			*/
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

#ifdef SKIP_HOSTNAME_VERIFICATION
			/*
			* If the site you're connecting to uses a different host name that what
			* they have mentioned in their server certificate's commonName (or
			* subjectAltName) fields, libcurl will refuse to connect. You can skip
			* this check, but this will make the connection less secure.
			*/
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif

			/* Perform the request, res will get the return code */
			res = curl_easy_perform(curl);
			/* Check for errors */

			if (res != CURLE_OK){

				fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
				writeDetectionToDisk(preparedPost);
			}
			/*so is connection*/
			else {

				resendOldDetection();
			}
		

			/* always cleanup */
			curl_easy_cleanup(curl);
		}

		curl_global_cleanup();
	}

	return 0;
	//o trimit post si astept confirmare
}

void writeDetectionToDisk(std::string completeURL) {

	mutexFileDetection.lock();
	std::ofstream logDetection;
	logDetection.open("localDetection.txt", std::ofstream::out | std::ofstream::app);
	logDetection << completeURL << std::endl;
	logDetection.close();
	mutexFileDetection.unlock();
}

void resendOldDetection() {

	std::vector < std::string> backupURL;
	std::ifstream filestr;
	std::string UrltoSent;
	
	mutexResendDetection.lock();
	filestr.open("localDetection.txt", std::ios::in);
	std::string line;
	if (filestr.is_open()) {

	while (getline(filestr, line)) {
		
		std::istringstream iss(line); // access line as a stream
		iss >> UrltoSent; // no need to read further
		backupURL.push_back(UrltoSent);
		}
	}
	filestr.close();
	bool interrupted = false;
	for (std::vector<std::string>::iterator it = backupURL.begin(); it != backupURL.end(); ++it) {

		if (ResendDetection(*it)) {

			backupURL.erase(backupURL.begin());
			--it;
		}
		else {
			interrupted = true;
			std::ofstream out;
			out.open("localDetection.txt");
			for (std::vector<std::string>::iterator it = backupURL.begin(); it != backupURL.end(); ++it) {
				out << *it;

			}
			out.close();

			break;
		}

	}
	if (!interrupted) {

		std::ofstream logDetection;
		logDetection.open("localDetection.txt", std::ofstream::out);
		logDetection.close();
	}
	mutexResendDetection.unlock();
}

bool ResendDetection(std::string detectionDataPost) {

	CURL *curl;
	CURLcode res;
	std::string completeURL = current_BACKOFFICEURL + "/api/saveImpression/";
	curl_global_init(CURL_GLOBAL_DEFAULT);

	//RpcStringFreeA((RPC_CSTR*)&preparedPost);
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, completeURL);
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, detectionDataPost);
#ifdef SKIP_PEER_VERIFICATION
		/*
		* If you want to connect to a site who isn't using a certificate that is
		* signed by one of the certs in the CA bundle you have, you can skip the
		* verification of the server's certificate. This makes the connection
		* A LOT LESS SECURE.
		*
		* If you have a CA cert for the server stored someplace else than in the
		* default bundle, then the CURLOPT_CAPATH option might come handy for
		* you.
		*/
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

#ifdef SKIP_HOSTNAME_VERIFICATION
		/*
		* If the site you're connecting to uses a different host name that what
		* they have mentioned in their server certificate's commonName (or
		* subjectAltName) fields, libcurl will refuse to connect. You can skip
		* this check, but this will make the connection less secure.
		*/
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */

		if (res != CURLE_OK){

			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));

			/* always cleanup */
			curl_easy_cleanup(curl);


			curl_global_cleanup();
			return false;
		}
		else {

			/* always cleanup */
			curl_easy_cleanup(curl);


			curl_global_cleanup();
			return true;
		}
	}
}

int postKeepAlive() {

	IPv4 MachineIPv4;
	std::string preparedPost;
	RegisterInfoNow.Version = GetVersionPlatform();
	getMyIP(MachineIPv4);
	RegisterInfoNow.MachineIPv4String = std::to_string(MachineIPv4.b1) + "." + std::to_string(MachineIPv4.b2) + "." + std::to_string(MachineIPv4.b3) + "." + std::to_string(MachineIPv4.b4);

	preparedPost = "uuid=" + RegisterInfoNow.UID + "&name=Intel Compute Stick" + "&platform=" + RegisterInfoNow.Version + "&ip=" + RegisterInfoNow.MachineIPv4String;

	std::string restURL = "/api/keepAlive/";
	std::string completeURL = current_BACKOFFICEURL + restURL + preparedPost;
	CURL *curl;
	CURLcode res;
	struct MemoryStruct chunk;

	chunk.memory = (char *)malloc(1);  /* will be grown as needed by the realloc above */
	chunk.size = 0;    /* no data at this point */

	curl_global_init(CURL_GLOBAL_DEFAULT);

	//RpcStringFreeA((RPC_CSTR*)&preparedPost);
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, completeURL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

		/* we pass our 'chunk' struct to the callback function */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, preparedPost);
#ifdef SKIP_PEER_VERIFICATION
		/*
		* If you want to connect to a site who isn't using a certificate that is
		* signed by one of the certs in the CA bundle you have, you can skip the
		* verification of the server's certificate. This makes the connection
		* A LOT LESS SECURE.
		*
		* If you have a CA cert for the server stored someplace else than in the
		* default bundle, then the CURLOPT_CAPATH option might come handy for
		* you.
		*/
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

#ifdef SKIP_HOSTNAME_VERIFICATION
		/*
		* If the site you're connecting to uses a different host name that what
		* they have mentioned in their server certificate's commonName (or
		* subjectAltName) fields, libcurl will refuse to connect. You can skip
		* this check, but this will make the connection less secure.
		*/
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */

		int key[50] = { 0 };
		int value[50] = { 0 };
		int index = 1;
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
		}
		else  {

			printf("%lu bytes retrieved\n", (long)chunk.size);
			char *sPtr = nullptr;
			char *newTemp = nullptr;
			//sPtr = strtok(chunk.memory, ",");
			std::string newstring(chunk.memory);
			//newstring = "0:1, 2 : 144, 3 : 176, 4 : 10, 5 : 2, 6 : 20, 7 : 20, 8 : 40, 9 : 40, 10 : 10, 12 : 60, 13 : 0, 14 : 0, 15 : 1, status : ok";
			size_t pos = 0;
			std::string delimiter = ",";
			std::string token;

			while ((pos = newstring.find(delimiter)) != std::string::npos) {
				token = newstring.substr(0, pos);
				std::cout << token << std::endl;
				{
					size_t pos1 = 0;
					std::string delimiter1 = ":";
					std::string token1;

					while ((pos1 = token.find(delimiter1)) != std::string::npos) {

						token1 = token.substr(0, pos1);
						std::cout << token1 << std::endl;
						key[index] = std::stoi(token1);
						token.erase(0, pos1 + delimiter1.length());
						value[index] = std::stoi(token);
						std::cout << token << std::endl;
						++index;
					}
				}

				newstring.erase(0, pos + delimiter.length());
			}

			std::cout << newstring << std::endl;


			std::cout << std::endl;
			if (current_version < value[1]) {
				std::cout << std::endl << "------>>>> A venit o noua versiune:" << value[1];
				current_version = value[1];


				for (int j = 1; j <= index; j++) {

					switch (key[j]) {
					case 0:
						break;

					case 1:
						//current_WINDOWNAME = ssval;

						break;
					case 2:

						current_USER_CAP_PROP_FRAME_WIDTH = value[j];

						break;
					case 3:

						current_USER_CAP_PROP_FRAME_HEIGHT = value[j];

						break;
					case 4:

						current_SCALEFACTOR = ((double)value[j] / 100 + 1);

						break;
					case 5:

						current_MINIMUMNEIGBOURS = value[j];

						break;
					case 6:

						current_minXSizeSE = value[j];

						break;
					case 7:

						current_minYSizeSE = value[j];

						break;
					case 8:

						current_DISTANCEFACETOBEDETECTEDX = value[j];

						break;
					case 9:

						current_DISTANCEFACETOBEDETECTEDY = value[j];

						break;
					case 10:

						current_MINIMUMFACEAPPARITION = value[j];

						break;
					case 11:

						//current_BACKOFFICEURL = ssval;

						break;
					case 12:

						current_FRAMESUNTILDELETEDETECTION = value[j];

						break;
					case 13:

						current_drawDetection = value[j];

						break;
					case 14:

						current_displayVideo = value[j];

						break;
					case 15:
						current_Notifybackoffice = value[j];
						break;
					case 17:
						current_minutesToSendLogFile = value[j];
					case 20:
						current_secToSendKeepAlive = value[j];
					default:
						break;
					}
				}
				//write on disk
				setnewVersion(true);
				saveConfiguration();

			}

			/* always cleanup */
			curl_easy_cleanup(curl);
		}
		free(chunk.memory);
		curl_global_cleanup();

		return 0;
		//o trimit post si astept confirmare
	}
}

int postFileLog(std::string stringToSend) {

		std::string completeURL;	
		std::string preparedPost = "uuid=" + RegisterInfoNow.UID + "&string=" + stringToSend;
		std::string restURL = "/api/saveVideo/";
		completeURL = current_BACKOFFICEURL + restURL;
		CURL *curl;
		CURLcode res;

		curl_global_init(CURL_GLOBAL_DEFAULT);

		curl = curl_easy_init();
		if (curl) {
			curl_easy_setopt(curl, CURLOPT_URL, completeURL);
			curl_easy_setopt(curl, CURLOPT_POST, 1);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, preparedPost);
#ifdef SKIP_PEER_VERIFICATION
			/*
			* If you want to connect to a site who isn't using a certificate that is
			* signed by one of the certs in the CA bundle you have, you can skip the
			* verification of the server's certificate. This makes the connection
			* A LOT LESS SECURE.
			*
			* If you have a CA cert for the server stored someplace else than in the
			* default bundle, then the CURLOPT_CAPATH option might come handy for
			* you.
			*/
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

#ifdef SKIP_HOSTNAME_VERIFICATION
			/*
			* If the site you're connecting to uses a different host name that what
			* they have mentioned in their server certificate's commonName (or
			* subjectAltName) fields, libcurl will refuse to connect. You can skip
			* this check, but this will make the connection less secure.
			*/
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif

			/* Perform the request, res will get the return code */
			res = curl_easy_perform(curl);
			/* Check for errors */

			if (res != CURLE_OK){

				fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));

			/* always cleanup */
			curl_easy_cleanup(curl);
		}

		curl_global_cleanup();
	}

	return 0;
	//o trimit post si astept confirmare
}