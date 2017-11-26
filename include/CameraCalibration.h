// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

#ifndef INCLUDE_CAMERACALIBRATION_H_
#define INCLUDE_CAMERACALIBRATION_H_

#include <opencv2/imgproc/imgproc.hpp>

#include "KFilter.h"
#include "PoseDetector.h"
#include "UDPclient.h"

class CameraCalibration {
public:
	CameraCalibration();
	~CameraCalibration();

	bool averageImage(cv::Mat input, int numFramesTotal);
	void fitTrack();
	void createWindows();
	void destroyWindows();
	void show();

	void transformToWorld(double* x, double* y, double* phi);
	void transformToCamera(double* x, double* y, double* phi);

	void transformToWorld(double* length);

	cv::Mat _averaged;
	cv::Mat _trackbackground;
	cv::Mat _trackmask;
	cv::Mat _threshed;
	cv::Mat _maskedTrack;

	double factorB;
	double factorG;
	double factorR;

private:
	void drawTrack();

	// count how many images the averaging has already considered
	int _avCount;

	// following 5 parameters have to be specified by hand

	// number of mats (piles) in x/y direction
	int _numMatsX;
	int _numMatsY;
	// size of a (quadratic) mat in meters
	double _sizeMatM;
	// center of track in meters in track coordinate system
	double _cxm;
	double _cym;
	double offsetXMeter;
	double offsetYMeter;

	// center of track in pixel in camera coordinate system
	double _cxp;
	double _cyp;

	// scale factor (meters per pixel)
	double _mPerP;
	// angle of the track in the camera system
	double _angle;

	// window names...
	std::string _w_threshed;
	std::string _w_averaged;
	std::string _w_maskedTrack;
};

#endif  // INCLUDE_CAMERACALIBRATION_H_
