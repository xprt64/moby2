
#include "VideoModule.h"
#include "NetworkModule.h"
/*remove the headers*/
#include <iostream>

#include <time.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

int main(){

	time_t ltime;
	time(&ltime);
	std::cout << ltime<<std::endl;
	printf("Time in seconds since UTC 1/1/70:\t%ld\n", ltime);

	FILE *f = fopen("localDetection.txt", "a");
	if (f != NULL)
	{
		fclose(f);
	}

	RegisterApplication();
	ConfigureParameters();

	if(RunVideo() == -1)
		exit(1);

}