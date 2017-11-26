// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

#ifndef KFILTER_H_
#define KFILTER_H_

#define VAR_ACCEL       (double)(10.0f*10.0f)
#define VAR_AROT        (double)((20.0f*M_PI)*(20.0f*M_PI))
#define VAR_MES_POS     (double)(0.01f*0.01f)
#define VAR_MES_ANGLE   (double)((5.0f*3.1415f/180.0f)*(5.0f*3.1415f/180.0f))
#define P0              (double)10

//#define VAR_ACCEL     (double)10.0
//#define VAR_AROT      (double)1.0
//#define VAR_MES_POS       (double)2.0e-7
//#define VAR_MES_ANGLE (double)4.0e-4
//#define P0                (double)10

#include "opencv2/video/tracking.hpp"
#include "Common.h"

// This class implements a openCV Kalman Filter for pose tracking.
// One instance per car, internal tracking of pose history!
class KFilter {

public:
    // Constructor
    KFilter();

    // Only needs xPos, yPos and Psi and detection success bool.
    // Overrides them with filtered Values and additionally provides the derivatives.
    // Returns data status (0: everything ok, 1: outlier, 2: detection failure.
    int filter(double* xPos, double* xDer, double* yPos, double* yDer, double* psiPos, double* psiDer, bool detectionSuccess);

    // Return info string.
    std::string getInfo();

private:
    // internal openCV filter
    cv::KalmanFilter KF;
};

#endif // KFILTER_H_
