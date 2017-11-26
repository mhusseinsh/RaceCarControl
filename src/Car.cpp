// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

#include "../include/Car.h"

#include <cstdio>
#include <iostream>
#include <sstream>

#include "../include/Common.h"

#include <fstream>

// ____________________________________________________________________________
Car::Car() {
	// pose data
	x = 0;
	y = 0;
	psi = 0;
	// derivatives of pose data
	xD = 0;
	yD = 0;
	psiD = 0;

	x2_old = 0;
	y2_old = 0;
	psi2_old = 0;
	x2 = 0;
	y2 = 0;
	psi2 = 0;
	xD2 = 0;
	yD2 = 0;
	psiD2 = 0;
	dataStatus2 = -1;

	// keep track of failures
	numOutliers = 0;
	numDetectionFails = 0;
	dataStatus = -1;

	timeStamp = -1;

	color = Color::INVALID;

	ang_raw = 0;
	ang_raw_old = 0;
	ang_measured_old = 0;

	initialized = false;
	toBeSent = false;
	detectionSuccess = false;
}

// ____________________________________________________________________________
Car::~Car() {
	dataout.close();
}

// ____________________________________________________________________________
void Car::initialize(Color carColor, int portNumber){
	color = carColor;
	toControl = UDPclient(portNumber);
	filter = KFilter();
	filter2 = KFilter();
	initialized = true;
  dataout.open("../labels.txt");
	dataout.precision(10);
	dataout << "\n" << "Counter" <<  "," << "Color" << "," << "x" << "," << "y" << ","
       << "psi";
}

// ____________________________________________________________________________
void Car::update(Box box, std::string obstacleString){
	if (!initialized) return;
	detectionSuccess = false;
	bool printToFile = false;
	if (box.color == color) {
		detectionSuccess = true;
		x = box.x;
		y = box.y;
		psi = box.angle;
		refineAngle();
		toBeSent = true;
		printToFile = true;

		rx = x;
		ry = y;
		rpsi= psi;
	}

	x2_old = x2;
	y2_old = y2;
	psi2_old = psi2;
	x2 = x;
	y2 = y;

	dataStatus = filter.filter(&x, &xD, &y, &yD, &psi, &psiD, detectionSuccess);
	psi2 = angleFromVelocity(xD, yD);

	if (sqrt(xD*xD + yD*yD) < 0.01) psi2 = 0;
	if (psi2 < 0) psi2 += 2*M_PI;
	int nw = floor(psi2_old / (2*M_PI));
	psi2 = psi2 + nw*2*M_PI;
	if (psi2 - psi2_old < -M_PI) {
		psi2 = psi2 + 2*M_PI;
	} else if (psi2 - psi2_old > M_PI) {
		psi2 = psi2 - 2*M_PI;
	}

	dataStatus2 = filter2.filter(&x2, &xD2, &y2, &yD2, &psi2, &psiD2, detectionSuccess);

	std::cout << "\ndetectionSuccess: " << detectionSuccess;
	switch (dataStatus) {
	case 0: break;
	case 1: ++numOutliers; break;
	case 2: ++numDetectionFails; break;
	}
	timestamp(&timeStamp);
	printData();
	obstacles = obstacleString;
	if (toBeSent) send();

	if(printToFile) {
    if(!cnnDatasetMode) {
  		dataout << "\n" << timeStamp << "\t" << dataStatus << "\t" << dataStatus2 << "\t"
  				<< rx << "\t" << x << "\t" << x2 << "\t" << xD << "\t" << xD2 << "\t"
  				<< ry << "\t" << y << "\t" << y2 << "\t" << yD << "\t" << yD2 << "\t"
  				<< rpsi << "\t" << psi << "\t" << psi2 << "\t" << psiD << "\t" <<  psiD2;
    }
    else {
      if(cnnCapture && calibrationDone)
      {
        dataout << "\n" << "Data_00" << cnnCounter/100 << "," << colorToString(color) << "," << x 
        << "," << y << "," << psi;
        cnnCapture = false;
      }
    }
	}
}

void Car::send() {
	// wrap up the packet for the control process and send it
	char messageToControl[BUFLEN] = {0};
	//snprintf(messageToControl, BUFLEN, "%.17g, %d, %.17g, %.17g, %.17g, %.17g, %.17g, %.17g,%.17g,%.17g,%.17g;%s", timeStamp, detectionSuccess, x, xD, y, yD, psi, psiD, rx, ry, rpsi, obstacles.c_str());
	snprintf(messageToControl, BUFLEN, "%.17g, %d, %.17g, %.17g, %.17g, %.17g, %.17g, %.17g,%.17g,%.17g,%.17g;%s", timeStamp, detectionSuccess, x2, xD2, y2, yD2, psi2, psiD2, rx, ry, rpsi, obstacles.c_str());
	//file << timeStamp << "\t" << x << "\t" << xD << "\t" << y << "\t" << yD << "\t" << psi << "\t" << psiD << "\n";
	toControl.send(messageToControl);
	std::cout << "\nUDP message size: " << strlen(messageToControl);
	//printf("\n%s: %s", colorToString(color).c_str(), messageToControl);
}

void Car::printData() {
	std::cout << "\n\nColor:  " << colorToString(color);
	std::cout << "\ntime:  " << timeStamp;
	std::cout << "\nx:    " << x;
	std::cout << "\nxD:   " << xD;
	std::cout << "\ny:    " << y;
	std::cout << "\nyD:   " << yD;
	std::cout << "\npsi:  " << psi;
	std::cout << "\npsiD: " << psiD;
	std::cout << "\ndata Status     : " << dataStatus;
	std::cout << "\n#detection fails: " << numDetectionFails;
	std::cout << "\n#outliers       : " << numOutliers;
	std::cout << "\n\n";
  // dataout << "\n" << colorToString(color) << "\t" << timeStamp << "\t" << x 
  //   << "\t" << y << "\t" << psi;
}

void Car::resetAngle() {
	psi = 0.0;
	ang_raw_old = 0.0;
	ang_raw = 0.0;
	ang_measured_old = 0.0;
}

void Car::refineAngle() {

	// save historic values and update raw angle
	ang_raw_old = ang_raw;

	// openCV::RotatedRect.angle is measured in degrees between ]-90,0[
	// which is random and ambivalent (see below)
	ang_raw = psi;

	// half winding index nWind
	// how often has car turned (by half) around its own axis?
	// it has to be estimated by the measured value since
	// the winding index is not consistent with the filtered value
	int nWind = round(ang_measured_old / M_PI);

	// check if an "angular jump" has happened (boxes only have angular range of 180°)
	double dif = ang_raw - ang_raw_old;
	std::cout << std::endl << "dif = " << dif;

	if (dif > M_PI/2.0) {
		nWind--;
	} else if (dif < (-M_PI/2.0)) {
		nWind++;
	}

	std::cout << std::endl << "winding number = " << nWind;
	std::cout << std::endl << "car angle = " << ang_raw;
	std::cout << std::endl << "previous unwound angle = " << ang_measured_old;

	// calculate absolute angle with this information
	// the angle is now unambiguous an continuous ]-inf,inf[
	// which is important for Kalman filtering!
	double angle = ang_raw + nWind*M_PI;
	ang_measured_old = angle;

	// because of openCV coordinate system the angle's sign has to be inverted
	psi = angle;
	std::cout << std::endl << "current unwound angle = " << psi;

}

double Car::angleFromVelocity(double vx, double vy) {
	return atan2(vy, vx);

	// following code works perfectly, but i found an easier way (atan2) after almost an hour reinventing the wheel lol
	/*
	if (vx == 0) {
		if (vy < 0) {
			return -M_PI / 2;
		} else {
			return M_PI / 2;
		}
	}

	if (vy == 0) {
			if (vx < 0) {
				return M_PI;
			} else {
				return 0;
			}
		}

	double rawAng = atan(fabs(vy / vx));

	if (vx > 0) {
		if (vy < 0) {
			return -rawAng;
		} else {
			return rawAng;
		}
	} else {
		if (vy < 0) {
			return -M_PI + rawAng;
		} else {
			return M_PI - rawAng;
		}
	}
	 */
}

