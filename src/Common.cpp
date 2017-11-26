// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

#ifndef INCLUDE_COMMONFUNCTIONS_H_
#define INCLUDE_COMMONFUNCTIONS_H_

#include <time.h>
#include <stdlib.h>
#include <sys/time.h>
#include <cmath>
#include <iostream>

#include <opencv2/highgui/highgui.hpp>

#include "../include/Common.h"

int mouseX = 0;
int mouseY = 0;
bool calibrationDone = false;
bool cnnDatasetMode = true;
int cnnCounter = 100;
bool cnnCapture = false;

// get a timestamp in microseconds
//_____________________________________________________________________________
long microtime() {
	timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return (ts.tv_sec * 1000000000 + ts.tv_nsec) / 1000;
}

// this one is needed for the ACADO control algorithm...
void timestamp(double *y){
	//timeval t;
	//gettimeofday(&t, NULL);
	timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	*y = fmod((double)ts.tv_sec + (double)ts.tv_nsec/1000000000.0,1.0e5);
}

std::string colorToString(Color color) {
	std::string colorString = "invalid";
	switch (color) {
	case Color::INVALID: break;
	case Color::RED: colorString = "red"; break;
	case Color::BLUE: colorString = "blue"; break;
	case Color::GREEN: colorString = "green"; break;
	case Color::YELLOW: colorString = "yellow"; break;
	case Color::WHITE: colorString = "white"; break;
	case Color::OBSTACLECOLORED: colorString = "obstaclecolored"; break;
	}
	return colorString;
}


void MouseCallBack(int event, int x, int y, int flags, void* obj){
	if  ( event == cv::EVENT_LBUTTONDOWN ) {
		mouseX = x;
		mouseY = y;
		std::cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
	}
	if  ( event == cv::EVENT_RBUTTONDOWN ) {
		calibrationDone = true;
	}
}

#endif // INCLUDE_COMMONFUNCTIONS_H_
