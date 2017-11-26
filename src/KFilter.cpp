// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video/tracking.hpp"
#include <iostream>
#include <stdio.h>
#include <ctime>
#include <cmath>

using namespace cv;
using namespace std;

#include "../include/KFilter.h"

string filter_info_string;

//_____________________________________________________________________________
KFilter::KFilter( )
{
	KF = KalmanFilter(6, 3, 0, CV_64F);
	double t = PERIOD/1.0e6;

	KF.transitionMatrix = Mat(6, 6, CV_64F);
	setIdentity(KF.transitionMatrix);
	KF.transitionMatrix.at<double>(0, 1)=t;
	KF.transitionMatrix.at<double>(2, 3)=t;
	KF.transitionMatrix.at<double>(4, 5)=t;

	KF.processNoiseCov = Mat(6, 6, CV_64F);
	KF.processNoiseCov.setTo(0);
	KF.processNoiseCov.at<double>(0,0)=t*t*t*t/4*VAR_ACCEL;
	KF.processNoiseCov.at<double>(0,1)=t*t*t/2*VAR_ACCEL;
	KF.processNoiseCov.at<double>(1,0)=t*t*t/2*VAR_ACCEL;
	KF.processNoiseCov.at<double>(1,1)=t*t*VAR_ACCEL;
	KF.processNoiseCov.at<double>(2,2)=t*t*t*t/4*VAR_ACCEL;
	KF.processNoiseCov.at<double>(2,3)=t*t*t/2*VAR_ACCEL;
	KF.processNoiseCov.at<double>(3,2)=t*t*t/2*VAR_ACCEL;
	KF.processNoiseCov.at<double>(3,3)=t*t*VAR_ACCEL;
	KF.processNoiseCov.at<double>(4,4)=VAR_AROT*t*t*t*t/4;
	KF.processNoiseCov.at<double>(4,5)=VAR_AROT*t*t*t/2;
	KF.processNoiseCov.at<double>(5,4)=VAR_AROT*t*t*t/2;
	KF.processNoiseCov.at<double>(5,5)= t*t*VAR_AROT;

	KF.measurementNoiseCov = Mat(3, 3, CV_64F);
	setIdentity(KF.measurementNoiseCov);
	KF.measurementNoiseCov.at<double>(0,0)=VAR_MES_POS;
	KF.measurementNoiseCov.at<double>(1,1)=VAR_MES_POS;
	KF.measurementNoiseCov.at<double>(2,2)=VAR_MES_ANGLE;

	KF.errorCovPre = Mat(6, 6, CV_64F);
	setIdentity(KF.errorCovPre);
	KF.errorCovPre *= P0;

	KF.measurementMatrix = Mat(3, 6, CV_64F);
	KF.measurementMatrix.setTo(0);
	KF.measurementMatrix.at<double>(0,0)=1;
	KF.measurementMatrix.at<double>(1,2)=1;
	KF.measurementMatrix.at<double>(2,4)=1;

}

//_____________________________________________________________________________
int KFilter::filter(double* xPos, double* xDer, double* yPos, double* yDer, double* psiPos, double* psiDer, bool detectionSuccess) {

	Mat_<double> measurement(3,1);

	// assign the new measurements
	measurement.at<double>(0) = *xPos;
	measurement.at<double>(1) = *yPos;
	measurement.at<double>(2) = *psiPos;

	Mat prediction = KF.predict();
	Mat estimation = KF.correct(measurement);

	stringstream info_stream;

	info_stream << "\nmeasured x Position:      \t" << measurement.at<double>(0);
	info_stream << "\nmeasured y Position:      \t" << measurement.at<double>(1);
	info_stream << "\nmeasured psi:             \t" << measurement.at<double>(2);
	/*
	info_stream << "\nmeasured x Derivative:    \t" << measurement.at<double>(3);
	info_stream << "\nmeasured y Derivative:    \t" << measurement.at<double>(4);
	info_stream << "\nmeasured psi Derivative:  \t" << measurement.at<double>(5);
	*/

	info_stream << "\npredicted x Position:     \t" << prediction.at<double>(0);
	info_stream << "\npredicted y Position:     \t" << prediction.at<double>(1);
	info_stream << "\npredicted psi:            \t" << prediction.at<double>(2);
	info_stream << "\npredicted x Derivative:   \t" << prediction.at<double>(3);
	info_stream << "\npredicted y Derivative:   \t" << prediction.at<double>(4);
	info_stream << "\npredicted psi Derivative: \t" << prediction.at<double>(5);

	info_stream << "\nestimated x Position:     \t" << estimation.at<double>(0);
	info_stream << "\nestimated y Position:     \t" << estimation.at<double>(1);
	info_stream << "\nestimated psi:            \t" << estimation.at<double>(2);
	info_stream << "\nestimated x Derivative:   \t" << estimation.at<double>(3);
	info_stream << "\nestimated y Derivative:   \t" << estimation.at<double>(4);
	info_stream << "\nestimated psi Derivative: \t" << estimation.at<double>(5);

	filter_info_string = info_stream.str();

	if (detectionSuccess) {

		// detection succeeded, now check for outliers

		if (fabs (measurement.at<double>(0) - prediction.at<double>(0)) < 0.01
		 && fabs (measurement.at<double>(1) - prediction.at<double>(2)) < 0.01
		 && fabs (measurement.at<double>(2) - prediction.at<double>(4)) < (M_PI / 18.0)) {

			// everything is fine, use estimation

			*xPos 	= estimation.at<double>(0);
			*xDer 	= estimation.at<double>(1);
			*yPos 	= estimation.at<double>(2);
			*yDer 	= estimation.at<double>(3);
			*psiPos  = estimation.at<double>(4);
			*psiDer  = estimation.at<double>(5);

			return 0;

		} else {

			// outlier has occured, use prediction

			*xPos 	= prediction.at<double>(0);
			*xDer 	= prediction.at<double>(1);
			*yPos 	= prediction.at<double>(2);
			*yDer 	= prediction.at<double>(3);
			*psiPos  = prediction.at<double>(4);
			*psiDer  = prediction.at<double>(5);

			return 1;
		}

	}

	// detection failed, use prediction

	*xPos 	= prediction.at<double>(0);
	*xDer 	= prediction.at<double>(1);
	*yPos 	= prediction.at<double>(2);
	*yDer 	= prediction.at<double>(3);
	*psiPos  = prediction.at<double>(4);
	*psiDer  = prediction.at<double>(5);

	return 2;
}

//_____________________________________________________________________________
string KFilter::getInfo() {
	return filter_info_string;
}

