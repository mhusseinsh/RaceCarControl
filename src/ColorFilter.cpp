// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

#include "../include/ColorFilter.h"

//_____________________________________________________________________________
int ColorFilter::filter(Mat input) {

        split(input, channels);

        r = channels[2];
        g = channels[1];
        b = channels[0];

        bitwise_and(r, b, rnb);
        bitwise_and(rnb, g, rnbng);

        absdiff(r, rnbng, r_);
        absdiff(g, rnbng, g_);
        absdiff(b, rnbng, b_);

        bitwise_not(b, b_n);
        bitwise_not(g, g_n);
        bitwise_not(r, r_n);

        bitwise_and(r_, g_n, red);
        bitwise_and(g_, r_n, green);
        bitwise_and(g_, r_, yellow);
        blue = b_;

    return 0;
}

//_____________________________________________________________________________
Mat ColorFilter::getRed() {
    return red;
}

//_____________________________________________________________________________
Mat ColorFilter::getGreen() {
    return green;
}

//_____________________________________________________________________________
Mat ColorFilter::getBlue() {
    return blue;
}

//_____________________________________________________________________________
Mat ColorFilter::getYellow() {
    return yellow;
}


