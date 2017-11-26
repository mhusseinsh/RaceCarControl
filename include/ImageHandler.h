// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

#ifndef IMAGEHANDLER_H_
#define IMAGEHANDLER_H_

#include <opencv2/core/core.hpp>
#include <map>

#include "Box.h"
#include "CameraCalibration.h"
#include "CameraHandler.h"
#include "Common.h"
#include "PoseDetector.h"

class Car;

// This class is for handling Images.
class ImageHandler {

public:
	ImageHandler();
	void getBoxes();
	void initialize();
	void preconditionImage();
	void detectBoxes();
	Color checkColor(int x, int y);
	void show();
	void drawCar(Car* car);

	cv::Mat image;
	cv::Mat original;
	cv::Mat bgsubtracted;
	cv::Mat channels[4];
	std::map<Color,Box> cars;
	//std::vector<Box> obstacles;
	//std::vector<cv::RotatedRect> obstacles;
	std::string obstacles;

private:
	CameraHandler camera;
	CameraCalibration calibration;
	PoseDetector detector;

	double factorB;
	double factorG;
	double factorR;

	bool display;
};

#endif // IMAGEHANDLER_H_
