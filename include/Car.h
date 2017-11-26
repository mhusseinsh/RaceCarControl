// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

#ifndef INCLUDE_CAR_H_
#define INCLUDE_CAR_H_

#include "Common.h"
#include "KFilter.h"
#include "UDPclient.h"
#include "Box.h"

#include <iostream>
#include <fstream>
#include <string>

class Box;

class Car {
public:
	Car();
	~Car();
	void initialize(Color color, int portNumber);
	void update(Box box, std::string obstacles);
	void resetAngle();
	double angleFromVelocity(double vx, double vy);

	// pose data
	double x;
	double y;
	double psi;
	// derivatives of pose data
	double xD;
	double yD;
	double psiD;
	// pose data
	double rx;
	double ry;
	double rpsi;

	Color color;

	std::ofstream dataout;

private:
	void printData();
	void send();
	void refineAngle();

	KFilter filter;

	UDPclient toControl;

	// keep track of failures
	int numOutliers;
	int numDetectionFails;
	int dataStatus;

	double ang_raw;
	double ang_raw_old;
	double ang_measured_old;

	double timeStamp;

	bool initialized;
	bool toBeSent;
	bool detectionSuccess;

	std::string obstacles;

	// alternative angle calc
	KFilter filter2;
	double x2_old;
	double y2_old;
	double psi2_old;
	double x2;
	double y2;
	double psi2;
	double xD2;
	double yD2;
	double psiD2;
	int dataStatus2;
};

#endif // INCLUDE_CAR_H_
