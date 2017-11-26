// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

#include "../include/CameraHandler.h"

#include <opencv2/core/core.hpp>
#include <opencv2/core/operations.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <ctime>
#include <iostream>
#include <stdio.h>
#include <memory.h>

#include "/opt/XIMEA/include/xiApi.h"

using namespace std;
using namespace cv;

//_____________________________________________________________________________
CameraHandler::CameraHandler() {

    xiH = NULL;
    stat = XI_OK;
    memset(&ximg,0,sizeof(ximg));
    ximg.size = sizeof(XI_IMG);

}

//_____________________________________________________________________________
CameraHandler::~CameraHandler() {

}

//_____________________________________________________________________________
void CameraHandler::init() {
    DWORD numDev;
    xiGetNumberDevices(&numDev);
    stat = xiOpenDevice(0, &xiH);

    xiSetParamInt(xiH, XI_PRM_EXPOSURE, 9000);
    xiSetParamInt(xiH, XI_PRM_GAIN, 0);
    xiSetParamInt(xiH, XI_PRM_DOWNSAMPLING, 2);
    xiSetParamInt(xiH, XI_PRM_DOWNSAMPLING_TYPE, XI_BINNING);
    //xiSetParamInt(xiH, XI_PRM_BINNING_PATTERN, XI_BIN_BAYER);
    //xiSetParamInt(xiH, XI_PRM_BINNING_SELECTOR, XI_BIN_SELECT_SENSOR);
    //xiSetParamInt(xiH, XI_PRM_BINNING_VERTICAL, 3);
    //xiSetParamInt(xiH, XI_PRM_BINNING_HORIZONTAL, 3);
    xiSetParamInt(xiH, XI_PRM_IMAGE_DATA_FORMAT, XI_RGB32);

    xiStartAcquisition(xiH);
    ximg.size = sizeof(XI_IMG);

}

//_____________________________________________________________________________
void CameraHandler::kill() {

    xiStopAcquisition(xiH);
    xiCloseDevice(xiH);
}

//_____________________________________________________________________________
void CameraHandler::printInfo() {
    float exposureTime;
    float maxFrameRate;
    float downsamplingFactor;
    xiGetParamFloat(xiH, XI_PRM_FRAMERATE, &maxFrameRate);
    xiGetParamFloat(xiH, XI_PRM_DOWNSAMPLING, &downsamplingFactor);
    xiGetParamFloat(xiH, XI_PRM_EXPOSURE, &exposureTime);
    cout << "\nSome Camera Info__________________________________";
    cout << "\ndownsampling factor: " << downsamplingFactor;
    cout << "\nexposure time (us):  " << exposureTime;
    cout << "\nmax Framerate (Hz):  "  << maxFrameRate << "\n";
}

//_____________________________________________________________________________
void CameraHandler::setExposureTime(int exptime) {
    xiSetParamInt(xiH, XI_PRM_EXPOSURE, exptime);
}

//_____________________________________________________________________________
int CameraHandler::getExposureTime() {
    int exptime;
    xiGetParamInt(xiH, XI_PRM_EXPOSURE, &exptime);
    return exptime;
}

//_____________________________________________________________________________
Mat CameraHandler::getImage() {
    long time = clock();
    XI_RETURN status = xiGetImage(xiH, 100, &ximg);
    Mat img(Size(ximg.width, ximg.height), CV_8UC(4), ximg.bp);
    //cvtColor(img, img, CV_RGBA2RGB);
    resize(img, img, Size(), 0.5, 0.5);
    return img;
}


