// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

#ifndef POSEDETECTOR_H_
#define POSEDETECTOR_H_

class PoseDetector {

public:

    // Constructor
    PoseDetector();

    // Constructor (takes binary threshold, dependent of the car color)
    void initialize(int thresh);

    // Enable debug output including image display (SLOW!!!).
    static void setDebug(bool dbg);

    // Takes a monochromatic images and calculates orientation from box detection:
    // x,y in meters and angle in radians.
    // Returns true on detection success, false if no box detected.
    bool update(cv::Mat, double* x, double* y, double* phi);

    void resetAngle();

    // TODO: deprecated, clean up (mask and background subtraction done in main)
    void setMask(cv::Mat mask);
    // void setBackground(cv::Mat mask);

    // Returns info string.
    std::string getInfo();

private:
    // enable debug
    static bool debug;

    // Threshold for binary image generation, range: ]0,255[
    int color_thresh;

    // variables for estimation of correct angle
    double ang_raw;
    double ang_raw_old;
    double ang_measured_old;

    // info stream
    std::stringstream pdc_info;

    // fits a box to pixel blobs applying constraints area and side ratio
    bool detectBox(cv::Mat, cv::RotatedRect&);

    // constraints for box detection
    double minArea;
    double maxArea;
    double minSideRatio;
    double maxSideRatio;
};

#endif // POSEDETECTOR_H
