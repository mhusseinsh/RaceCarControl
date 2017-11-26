// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

#include <signal.h>
#include <stdio.h>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>

#include "../include/Box.h"
#include "../include/CarHandler.h"
#include "../include/Common.h"
#include "../include/ImageHandler.h"

bool running;
bool resetAngles;

void signalHandler(int signal);

//_____________________________________________________________________________
int main( int argc, const char** argv ) {

	/*
	Car car;
	for (int i = - 200; i <= 200; ++i) {
		double ang = i * (M_PI / 180);
		double vx = cos(ang);
		double vy = sin(ang);
		double angVel = car.angleFromVelocity(vx, vy);
		double dif = ang - angVel;
		//std::cout << "\nang raw: " << ang << "\tang vel:" << angVel << "\tvx: " << vx << "\tvy: " << vy;
		printf("\nang deg: %d", i);
		printf("\tang raw: %3.17f", ang);
		printf("\tang vel: %3.17f", angVel);
		printf("\tdiff: %3.17f", dif);
		printf("\tvx: %3.17f", vx);
		printf("\tvy: %3.17f", vy);
		if (fabs(dif) > 0.000001) printf(" <-------------------");
	}
	exit(0);
	*/

	CarHandler ch;
	// TODO: if a car is accessed which is not initialized, update() will access a nullpointed car object!!!
	//FIX THIS

	//ch.initCar(Color::WHITE, 10000);
	//ch.initCar(Color::BLUE, 10001);
	//ch.initCar(Color::GREEN, 10002);
	ch.initCar(Color::RED, 9930);
	//ch.initCar(Color::YELLOW, 10004);

	ImageHandler ih;
	ih.initialize();

	signal(SIGINT, signalHandler);

	running = true;
	resetAngles = false;

	int boxEmptyCount = 0;

	while(running) {

		if (resetAngles == true) {
			ch.resetAngles();
			resetAngles = false;
		}

		std::stringstream timingInfo;
		long starttime = microtime();
		long currenttime = starttime;

		ih.getBoxes();

		ch.update(ih.cars, ih.obstacles);

		timingInfo << "\n[microseconds] Image processing: " << microtime() - currenttime;
		currenttime = microtime();

		while(microtime() - starttime < PERIOD);
		timingInfo << "\n[microseconds] loop fill:   \t" << microtime() - currenttime;
		timingInfo << "\n[microseconds] loop time:   \t" << microtime() - starttime;
		std::cout << timingInfo.str();
	}
	ch.closeOutputFiles();
}

//_____________________________________________________________________________
// This function catches the "Ctrl+C" Keypress and cleanly exits after telling
// the control process to stop the motors and shutting down the camera.
void signalHandler(int signal){
	std::cout << "\n\nCaught Ctrl+C (signal %d)\n\n";
	std::cout << "q: quit\n";
	std::cout << "r: reset angles\n";
	char key = getchar();
	switch(key) {
	case 'q':
		running = false;
		break;
	case 'r':
		resetAngles = true;
		break;
	}
}
