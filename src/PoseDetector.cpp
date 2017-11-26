// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

#include <iostream>

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "../include/PoseDetector.h"
#include "../include/Common.h"

using namespace std;
using namespace cv;

bool PoseDetector::debug;

//_____________________________________________________________________________
PoseDetector::PoseDetector() {

    color_thresh = 100;

    // starting angles...
    // TODO: maybe make customizable
    ang_raw = 0;
    ang_raw_old = 0;
    ang_measured_old = 0;

    // initialize defaults
    // TODO: make customizable
    minArea = 100;
    maxArea = 400;
    minSideRatio = 0.25;
    maxSideRatio = 4.0;

    pdc_info.clear();
}

//_____________________________________________________________________________
void PoseDetector::initialize(int thresh) {

    // initialize mask and threshold
    color_thresh = thresh;

    pdc_info.clear();
}

//_____________________________________________________________________________
bool PoseDetector::detectBox(Mat input, RotatedRect &box) {

    vector<vector< cv::Point> > boxes;

    threshold(input, input, color_thresh, 255.0, THRESH_BINARY);

    if (debug) {
        std::stringstream name;
        name << "thresh " << color_thresh;
        namedWindow(name.str(), 1);
        imshow(name.str(), input);
        waitKey(1);
    }

    findContours(input, boxes, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

    for (int i = 0; i < boxes.size(); ++i) {
        RotatedRect rRect = minAreaRect(boxes[i]);
        int height = rRect.size.height;
        int width = rRect.size.width;
        int area = height * width;
        double ratio = (double)width / (double)height;
        if (area > minArea && area < maxArea && ratio > minSideRatio && ratio < maxSideRatio) {
            box = rRect;
            return true;
        }
    }
    return false;
}

//_____________________________________________________________________________
bool PoseDetector::update(Mat input, double* x, double* y, double* phi) {

    pdc_info.str(std::string());
    pdc_info.clear();

    Mat image = input.clone();

    RotatedRect boxMeasured;
    // GaussianBlur(image, image, Size(9,9), 100.0);
    bool boxDetected = detectBox(image, boxMeasured);

    if(!boxDetected) {
        return false;
    }

    double boxAngle = boxMeasured.angle;
    double boxX = boxMeasured.center.x;
    double boxY = boxMeasured.center.y;
    double boxWidth = boxMeasured.size.width;
    double boxHeight = boxMeasured.size.height;

    cout << "\nboxX: " << boxX << " / " << boxMeasured.center.x;
    cout << "\nboxY: " << boxX << " / " << boxMeasured.center.x;
    cout << "\nboxA: " << boxAngle << " / " << boxMeasured.angle;


    // save historic values and update raw angle
    ang_raw_old = ang_raw;

    // openCV::RotatedRect.angle is measured in degrees between ]-90,0[
    // which is random and ambivalent (see below)
    ang_raw = boxAngle;

    // correct angle to be parallel to the high side of the box (range now ]-90,90[)
    // is still ambivalent though
    if (boxHeight > boxWidth) {
        ang_raw += 90;
    }

    // restrict range from ]-90,90[ to [-90,90]
    // (exactly (-)90.0° causes problems with the winding index!)
    if(ang_raw == 90.0) {
        ang_raw = 89.9;
    } else if(ang_raw == -90.0) {
        ang_raw = -89.9;
    }

    // half winding index nWind
    // how often has car turned (by half) around its own axis?
    // it has to be estimated by the measured value since
    // the winding index is not consistent with the filtered value
    double nWind = round(ang_measured_old / 180.0);

    // check if an "angular jump" has happened (boxes only have angular range of 180°)
    double dif = ang_raw - ang_raw_old;
    if (dif > 90) {
        nWind--;
    } else if (dif < (-90)) {
        nWind++;
    }

    // calculate absolute angle with this information
    // the angle is now unambiguous an continuous ]-inf,inf[
    // which is important for Kalman filtering!
    double angle = ang_raw + nWind*180;
    ang_measured_old = angle;

    // because of openCV coordinate system the angle's sign has to be inverted
    // also the unit is converted to radians
    angle = -angle * M_PI / 180;

    // in pixels
    *x = boxX;
    *y = boxY;
    *phi = angle;

    return true;
}

// TODO: output some info...
//_____________________________________________________________________________
string PoseDetector::getInfo() {

    return pdc_info.str();
}

//_____________________________________________________________________________
void PoseDetector::setDebug(bool dbg) {
    debug = dbg;
}

//_____________________________________________________________________________
void PoseDetector::resetAngle() {
    ang_measured_old = 0.0;
    ang_raw_old = 0.0;
    ang_raw = 0.0;
}

