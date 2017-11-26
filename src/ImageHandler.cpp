// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

#include "../include/ImageHandler.h"

#include <opencv2/core/mat.hpp>
#include <opencv2/core/operations.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <stddef.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <sstream>

#include "../include/Car.h"

using namespace std;
using namespace cv;

double psiAfterBoxDetection;

ImageHandler::ImageHandler() {
}

void ImageHandler::getBoxes() {
	image = camera.getImage();
  if(calibrationDone && cnnDatasetMode) {
    if(cnnCounter % 100 == 0) {
      imwrite("../images/Data_00" + std::to_string(cnnCounter/100) + ".jpg", image);
      cnnCapture = true;
    }
  }
  cnnCounter++;
	detectBoxes();
	std::cout << "\n#cars found     : " << cars.size();
	std::cout << "\n#obstacles found: " << obstacles.size();
}

void ImageHandler::initialize() {
	// initialize camera
	camera.init();
	camera.printInfo();

	// do track calibration and wait for user to acknowledge
	calibration.createWindows();
	while (true) {
		if (calibrationDone) break;
		while(!calibration.averageImage(camera.getImage(), 100));
		calibration.fitTrack();
		calibration.show();
	}
	calibration.destroyWindows();

	display = false;
	display = true;

	if (display) {
		//namedWindow("white_balanced", 1);
		//namedWindow("bgsubtracted_masked", 1);
		namedWindow("image", 1);
	}
}

void ImageHandler::preconditionImage() {

	original = image.clone();

	Mat chans[4];

	int x = mouseX;
	int y = mouseY;

	image.convertTo(image, CV_64FC3);


	split(image, chans);


	int kernelRadius = 3;
	factorB = 0.0;
	factorG = 0.0;
	factorR = 0.0;
	for (int k = y-kernelRadius; k < y+kernelRadius+1; ++k) {
		for (int i = x-kernelRadius; i < x+kernelRadius+1; ++i) {
			factorB += 192.0 / chans[0].at<double>(k, i);
			factorG += 192.0 / chans[1].at<double>(k, i);
			factorR += 192.0 / chans[2].at<double>(k, i);
		}
	}
	factorB /= ((2*kernelRadius+1)*(2*kernelRadius+1));
	factorG /= ((2*kernelRadius+1)*(2*kernelRadius+1));
	factorR /= ((2*kernelRadius+1)*(2*kernelRadius+1));


	chans[0] *= factorB;
	chans[1] *= factorG;
	chans[2] *= factorR;


	merge(chans, 3, image);
	image.convertTo(image, CV_8UC3);

	//if (display) imshow("white_balanced", image);

	original = image.clone();

	image -= calibration._trackbackground;
	image &= calibration._trackmask;

	//if (display) imshow("bgsubtracted_masked", image);

	bgsubtracted = image.clone();
	split(image, channels);
	image = max(channels[0], channels[1]);
	image = max(image, channels[2]);
	threshold(image, image, 51, 255.0, THRESH_BINARY);
}

void ImageHandler::detectBoxes() {
	cars.clear();
	obstacles.clear();

	std::stringstream obstacleStringStream;

	preconditionImage();
	vector<vector< cv::Point> >  contours;
	findContours(image, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
	for (int i = 0; i < contours.size(); ++i) {
		RotatedRect rRect = minAreaRect(contours[i]);
		Color color = checkColor(rRect.center.x, rRect.center.y);

		psiAfterBoxDetection = rRect.angle;

		double height = 0;
		double width = 0;
		if (rRect.size.height > rRect.size.width) {
			height = rRect.size.height;
			width = rRect.size.width;
		} else {
			height = rRect.size.width;
			width = rRect.size.height;
		}

		calibration.transformToWorld(&height);
		calibration.transformToWorld(&width);
		if (height > 0.04 && width > 0.04) {
			double x = rRect.center.x;
			double y = rRect.center.y;
			double psi = rRect.angle;
			// correct angle to be parallel to the high side of the box (range now ]-90,90[)
			// is still ambivalent though
			if (rRect.size.height > rRect.size.width) {
				psi += 90;
			}
			// restrict range from ]-90,90[ to [-90,90]
			// (exactly (-)90.0° causes problems with the winding index!)
			//if(psi > 89.9) {
				//psi = 89.9;
			//} else if(psi < -89.9) {
				//psi = -89.9;
			//}
			psi *= M_PI / 180.0;
			psi = -psi;
			double blue, green, red;
			calibration.transformToWorld(&x, &y, &psi);
			if (psi < -1.569) {
			    psi += M_PI;
				//psi = -1.569;
            } else if (psi > 1.569) {
                psi -= M_PI;
                //psi = 1.569;
            }

			Point2f rect_points[4];
			rRect.points( rect_points );
			Scalar col = Scalar(0, 0, 0);
			if(color == Color::RED) col = Scalar(0,0,255);
			if(color == Color::GREEN) col = Scalar(0,255,0);
			if(color == Color::BLUE) col = Scalar(255,0,0);
			if(color == Color::YELLOW) col = Scalar(0,255,255);
			if(color == Color::WHITE) col = Scalar(255,255,255);
			if(color == Color::OBSTACLECOLORED) col = Scalar(100,255,255);
			if(color == Color::INVALID) col = Scalar(255,0,255);

			if ((height > 0.08 && width > 0.04 && height < 0.16 && width < 0.08) && (color == Color::BLUE || color == Color::RED || color == Color::GREEN)) {
				cars[color] = Box(x, y, psi, width, height, color);
				for(int j = 0; j < 4; j++) line(original, rect_points[j], rect_points[(j+1)%4], col, 2, 8);
			} else if (color == Color::OBSTACLECOLORED && height > 0.04 && width > 0.04) {
				//obstacles.push_back(Box(x, y, psi, width, height, color));
				Point2f points[4];
				rRect.points(points);
				double px0 = points[0].x;
				double px1 = points[1].x;
				double px2 = points[2].x;
				double px3 = points[3].x;
				double py0 = points[0].y;
				double py1 = points[1].y;
				double py2 = points[2].y;
				double py3 = points[3].y;
				double dummyPsi;
				calibration.transformToWorld(&px0, &py0, &dummyPsi);
				calibration.transformToWorld(&px1, &py1, &dummyPsi);
				calibration.transformToWorld(&px2, &py2, &dummyPsi);
				calibration.transformToWorld(&px3, &py3, &dummyPsi);
				obstacleStringStream << px0 << "," << py0 << "," << px1 << "," << py1 << "," << px2 << "," << py2 << "," << px3 << "," << py3 << ";";
				for(int j = 0; j < 4; j++) line(original, rect_points[j], rect_points[(j+1)%4], col, 2, 8);
			}
		}
	}
	obstacles = obstacleStringStream.str();
	if (display) {
		imshow("image", original);
		waitKey(1);
	}
}

Color ImageHandler::checkColor(int x, int y) {
	double blue = 0;
	double green = 0;
	double red = 0;
	for (int row = -2; row < 3; ++row) {
		for (int col = -2; col < 3; ++col) {
			blue  += channels[0].at<uchar>(y + row, x + col);
			green += channels[1].at<uchar>(y + row, x + col);
			red   += channels[2].at<uchar>(y + row, x + col);
		}
	}

	blue /= 25.0;
	green /= 25.0;
	red   /= 25.0;

	Color color = Color::INVALID;

	if (blue > 50 || green > 50 || red > 50) {
	//if (blue < 100 && green > 100 && red < 100) {
			color = Color::OBSTACLECOLORED;
	}
	if (blue > 70 && blue > red && blue > green) {
	//if (blue > 100 && green < 100 && red < 100) {
			//color = color = Color::BLUE;
	}
	if (red > 100.0 && red/blue > 3.0 && red/green > 3.0) {
	//if (blue < 100 && green < 100 && red > 100) {
				color = Color::RED;
	}
	if (green > 50.0 && green/blue > 1.25 && green/red > 1.25) {
	//if (blue < 100 && green > 100 && red < 100) {
			//color = Color::GREEN;
	}

	/*
	if (blue > 60.0 && red > 120.0 && green > 100.0) {
		color = Color::WHITE;
	}
	if (blue < 60.0 && red > 140.0 && green > 140.0) {
		color = Color::YELLOW;
	}
	*/
	return color;
}

void ImageHandler::show() {
	namedWindow("detections", 1);
	imshow("detections", original);
}

void ImageHandler::drawCar(Car* car) {
	Scalar col = Scalar(0, 0, 0);
	if(car->color == Color::RED) col = Scalar(0,0,255);
	if(car->color == Color::GREEN) col = Scalar(0,255,0);
	if(car->color == Color::BLUE) col = Scalar(255,0,0);
	if(car->color == Color::YELLOW) col = Scalar(0,255,255);
	if(car->color == Color::WHITE) col = Scalar(255,255,255);
	if(car->color == Color::INVALID) col = Scalar(255,0,255);
	double x0 = car->x;
	double y0 = car->y;
	double psi = car->psi;
	double x1 = car->x + 0.1 * cos(psi);
	double y1 = car->y + 0.1 * sin(psi);
	calibration.transformToCamera(&x0, &y0, &psi);
	double dummy;
	calibration.transformToCamera(&x1, &y1, &dummy);
	line(original, Point2f(x0, y0), Point2f(x1, y1), col, 2, 8);
}
