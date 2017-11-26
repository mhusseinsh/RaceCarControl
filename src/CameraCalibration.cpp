// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

// FOLLOWING CODE IS ONLY A PROOF OF CONCEPT FOR AUTOMATIC CAMERA CALIBRATION AND MASK GENERATION

// WORKS LIKE A CHARM BUT I HAVE TO CLEAN UP THAT MESS THOUGH...

#include <iostream>

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <map>

#include "../include/CameraCalibration.h"
#include "../include/TrackParser.h"
#include "../include/Common.h"

using namespace std;
using namespace cv;

CameraCalibration::CameraCalibration() {

	_w_threshed = "threshed";
	_w_averaged = "_averaged";
	_w_maskedTrack = "maskedTrack";

	_avCount = 0;

	_numMatsX = 5;
	_numMatsY = 4;
	_sizeMatM = 0.5;

	offsetXMeter = -2.0 * _sizeMatM;
	offsetYMeter = 0.5 * _sizeMatM;

	_cxm = offsetXMeter + (+static_cast<double>(_numMatsX)/2 * _sizeMatM);
	_cym = offsetYMeter + (-static_cast<double>(_numMatsY)/2 * _sizeMatM);

	_cxp = 0;
	_cyp = 0;
	_mPerP = -1;
	_angle = 0;

	factorB = 1.0;
	factorG = 1.0;
	factorR = 1.0;
}
// 375, 180
CameraCalibration::~CameraCalibration() {
	_averaged.release();
	_trackbackground.release();
	_trackmask.release();
}

bool CameraCalibration::averageImage(Mat input, int numFramesTotal) {
	Mat temp;

	input.convertTo(temp, CV_64FC3);

	if (_avCount == 0) {
		_averaged = (temp / (double)numFramesTotal);
	} else {
		_averaged += (temp / (double)numFramesTotal);
	}

	if (_avCount < 100) {
		++_avCount;
	}

	if (_avCount == 100) {

		Mat channels[4];
		split(_averaged, channels);

		int x = mouseX;
		int y = mouseY;
		factorB = 192.0 / channels[0].at<double>(y, x);
		factorG = 192.0 / channels[1].at<double>(y, x);
		factorR = 192.0 / channels[2].at<double>(y, x);

		vector<Mat> chan;
		chan.push_back(channels[0] * factorB);
		chan.push_back(channels[1] * factorG);
		chan.push_back(channels[2] * factorR);
		merge(chan,_averaged);

		_averaged.convertTo(_averaged, CV_8UC1);
		_trackbackground = _averaged.clone();
		_avCount = 0;
		return true;
	}

	return false;
}

void CameraCalibration::fitTrack() {

	Mat gaussed, channels[4], contours, contourmap;

	RNG rng(12345);
	GaussianBlur(_averaged, gaussed, Size(9, 9), 200, 0, BORDER_DEFAULT);
	split(gaussed, channels);

	double color_thresh = 90;
	double minArea = 8000;
	double maxArea = 300000;
	double minSideRatio = 0.66;
	double maxSideRatio = 1.5;

	vector<vector< cv::Point> > boxes;
	vector<Vec4i> hierarchy;

	contourmap = _averaged.clone();
	_trackmask = _averaged.clone();
	contourmap = Scalar(0,0,0);
	_trackmask = Scalar(0,0,0);

	Mat maximum;
	maximum = max(channels[0], channels[1]);
	maximum = max(maximum, channels[2]);

	threshold(maximum, _threshed, color_thresh, 255.0, THRESH_BINARY);

	// contours = threshed.clone();
	contours = _threshed.clone();

	findContours(contours, boxes, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, Point(0, 0) );

	int foundBoundaries = 0;
	double outHeight = 0;
	double outWidth = 0;
	double inHeight = 0;
	double inWidth = 0;
	double centerXPixel = 0;
	double centerYPixel = 0;
	double angle;

	for (int i = 0; i < boxes.size(); ++i) {
		RotatedRect rRect = minAreaRect(boxes[i]);
		double height = rRect.size.height;
		double width = rRect.size.width;
		double area = height * width;
		double ratio = (double)width / (double)height;
		//if (true) {
		if (area > minArea && area < maxArea && ratio > minSideRatio && ratio < maxSideRatio) {

			++foundBoundaries;
			Point2f points[4];
			rRect.points(points);
			for( int k = 0; k < 4; k++ ) {
				line(_threshed, points[k], points[(k+1)%4], Scalar(127,0,0), 1, 8 );
				line(_averaged, points[k], points[(k+1)%4], Scalar(255,0,0), 1, 8 );
				line(contours, points[k], points[(k+1)%4], Scalar(255,0,0), 1, 8 );
			}
			Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
			drawContours( contourmap, boxes, i, color, CV_FILLED, 8, hierarchy, 0, Point() );
			if (foundBoundaries == 1) {
				outHeight = height;
				outWidth = width;
				angle = rRect.angle;
				if (height > width) {
					angle += 90;
					outHeight = width;
					outWidth = height;
				}
				angle *= -(M_PI / 180.0);
				centerXPixel = rRect.center.x;
				centerYPixel = rRect.center.y;
			} else if (foundBoundaries == 2) {
				drawContours( _trackmask, boxes, i, Scalar(255,255,255), CV_FILLED, CV_AA, hierarchy, 0, Point() );
			} else if (foundBoundaries == 3) {
				drawContours( _trackmask, boxes, i, Scalar(0,0,0), CV_FILLED, CV_AA, hierarchy, 0, Point() );
			}
		}
	}
	double matSize = 0.45;
	double pixelPerMeter = (outWidth/_numMatsX + outHeight/_numMatsY) / (2 * _sizeMatM);
	_mPerP = 0.5 * _sizeMatM * (_numMatsX / outWidth + _numMatsY / outHeight);
	_cxp = centerXPixel;
	_cyp = centerYPixel;
	_angle = angle;

	// origin of the 5x4 mat (up left on screen, down right in reality)
	double originXPixel = centerXPixel - outWidth/2  *cos(angle) - outHeight/2 *sin(angle);
	double originYPixel = centerYPixel - outHeight/2 *cos(-angle) - outWidth/2  *sin(-angle);
	// origin of the track (0,0 of the reference)
	double originTrackXPixel = originXPixel + pixelPerMeter * (offsetXMeter *cos(angle)  + offsetYMeter  *sin(angle));
	double originTrackYPixel = originYPixel + pixelPerMeter * (offsetYMeter *cos(-angle) + offsetXMeter *sin(-angle));

	// draw the grid, so the user can verify
	for (int i = 0; i <= _numMatsY; ++i) {
		double px0 = offsetXMeter;
		double px1 = _numMatsX * _sizeMatM + offsetXMeter;
		double py0 = -i * _sizeMatM + offsetYMeter;
		double py1 = -i * _sizeMatM + offsetYMeter;
		double bla = 0;
		stringstream s;
		s << "(" << px0 << "/" << py0 << ")";
		transformToCamera(&px0, &py0, &bla);
		transformToCamera(&px1, &py1, &bla);
		line(_averaged, Point2f(px0, py0), Point2f(px1, py1), Scalar(0,0,255), 1, 8 );
		putText(_averaged, s.str(), Point2f(px0, py0), FONT_HERSHEY_PLAIN, 1,  Scalar(255,255,255), 1);
	}
	for (int i = 0; i <= _numMatsX; ++i) {
		double px0 = i * _sizeMatM + offsetXMeter;
		double px1 = i * _sizeMatM + offsetXMeter;
		double py0 = offsetYMeter;
		double py1 = -_numMatsY * _sizeMatM + offsetYMeter;
		double bla = 0;
		stringstream s;
		s << "(" << px0 << "/" << py0 << ")";
		transformToCamera(&px0, &py0, &bla);
		transformToCamera(&px1, &py1, &bla);
		line(_averaged, Point2f(px0, py0), Point2f(px1, py1), Scalar(0,0,255), 1, 8 );
		putText(_averaged, s.str(), Point2f(px0, py0), FONT_HERSHEY_PLAIN, 1,  Scalar(255,255,255), 1);
	}

	double origX = 0.0;
	double origY = 0.0;
	double origPhi = 0.0;
	transformToCamera(&origX, &origY, &origPhi);

	circle(_averaged, Point2f(origX, origY), 1, Scalar(255,0,0), 10);

	Mat d_kernel = getStructuringElement(MORPH_ELLIPSE, Size(20,20));
	Mat e_kernel = getStructuringElement(MORPH_ELLIPSE, Size(15,15));
	dilate(_trackmask, _trackmask, d_kernel);
	// with background subtraction it is better to not erode harsh so that the car
	// will not disappear beneath the mask
	erode(_trackmask, _trackmask, e_kernel);
	bitwise_and(_averaged, _trackmask, _maskedTrack);
}

void CameraCalibration::show() {
	/*
	_averaged.convertTo(_averaged, CV_64FC3);
	Mat channels[4];
	split(_averaged, channels);

	int x = mouseX;
	int y = mouseY;
	factorB = 255.0 / channels[0].at<double>(y, x);
	factorG = 255.0 / channels[1].at<double>(y, x);
	factorR = 255.0 / channels[2].at<double>(y, x);

	vector<Mat> chan;
	chan.push_back(channels[0] * factorB);
	chan.push_back(channels[1] * factorG);
	chan.push_back(channels[2] * factorR);
	merge(chan,_averaged);
	_averaged.convertTo(_averaged, CV_8UC1);
	*/

	imshow(_w_threshed, _threshed);
	imshow(_w_averaged, _averaged);
	imshow(_w_maskedTrack, _maskedTrack);
	waitKey(1);
}

void CameraCalibration::createWindows() {
	startWindowThread();
	namedWindow(_w_threshed, 1);
	namedWindow(_w_averaged, 1);
	namedWindow(_w_maskedTrack, 1);
    cv::setMouseCallback(_w_averaged, MouseCallBack, NULL);
}

void CameraCalibration::transformToWorld(double* x, double* y, double* phi) {
	// coordinates in pixels in camera system
	double xp = *x;
	double yp = *y;
	double phip = *phi;

	// coordinates in meters in world system
	double xm = _mPerP * ( (xp - _cxp) * cos(_angle) - (yp - _cyp) * sin(_angle) ) + _cxm;
	double ym = _mPerP * (-(xp - _cxp) * sin(_angle) - (yp - _cyp) * cos(_angle) ) + _cym;
	double phim = phip + _angle;

	*x = xm;
	*y = ym;
	*phi = phim;
}

void CameraCalibration::transformToWorld(double* length) {
	double l = *length;
	*length = l * _mPerP;
}

void CameraCalibration::transformToCamera(double* x, double* y, double* phi) {
	// coordinates in meters in world system
	double xm = *x;
	double ym = *y;
	double phim = *phi;

	// coordinates in pixels in camera system
	double xp = (1.0 / _mPerP) * ( (xm - _cxm) * cos(_angle) - (ym - _cym) * sin(_angle) ) + _cxp;
	double yp = (1.0 / _mPerP) * (-(xm - _cxm) * sin(_angle) - (ym - _cym) * cos(_angle) ) + _cyp;
	double phip = phip + _angle;

	*x = xp;
	*y = yp;
	*phi = phip;
}

void CameraCalibration::destroyWindows() {
	destroyWindow(_w_threshed);
	destroyWindow(_w_averaged);
	destroyWindow(_w_maskedTrack);
}
