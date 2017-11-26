// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

#ifndef CAMERAHANDLER_H_
#define CAMERAHANDLER_H_

#include "/opt/XIMEA/include/xiApi.h"
#include <opencv2/core/core.hpp>

// This class is for handling the XIMEA camera.
class CameraHandler {

public:
    CameraHandler();
    ~CameraHandler();

    // Open device and initialize settings.
    void init();

    // Close device.
    void kill();

    // Return most recent image in openCV format
    cv::Mat getImage();

    // Print Camera Info.
    void printInfo();

    void setExposureTime(int exptime);
    int getExposureTime();

private:
    XI_RETURN stat;

    // camera reference
    HANDLE xiH;

    // internal image format
    XI_IMG ximg;
};

#endif // CAMERAHANDLER_H_
