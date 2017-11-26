// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

/*
 * This program is dedicated to display the position of detected cars and reference.
 * Drawing is quite resource hungry in openCV and is therefore outsourced to an
 * own process
 */

#include <stdio.h>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "../include/UDPserver.h"

using namespace std;
using namespace cv;

float x2p(float x);
float y2p(float y);

//_____________________________________________________________________________
int main() {

    UDPserver us(9931);
    char buf[BUFLEN];

    Mat img = imread("pics/mask.tif");
    namedWindow("display", 1);
    waitKey(300);

    Mat image = img.clone();

    imshow("display", image);
    waitKey(10);

    int identifier = -1;
    int timestep = -1;
    float xPos = 0;
    float yPos = 0;
    float psi = 0;
    float xPosRef = 0;
    float yPosRef = 0;
    float psiRef = 0;
    float xPosActual = 0;
    float yPosActual = 0;
    float psiActual = 0;
    float xPosBlue = 0;
    float yPosBlue = 0;
    float psiBlue = 0;

    while (true) {

        image = img.clone();

        us.listen(buf);
        sscanf(buf, "%i, %i, %f, %f, %f", &identifier, &timestep, &xPos, &yPos, &psi);

        if (identifier == -2) {
            cout << "\n Got the order to end myself.";
            exit(0);
        }
        else if (identifier == -1) {
            cout << "\n Waiting for Data.";
            continue;
        }
        else if (identifier == 0) {
            xPosActual = xPos;
            yPosActual = yPos;
            psiActual = psi;
        }
        else if (identifier == 1) {
            xPosRef = xPos;
            yPosRef = yPos;
            psiRef = psi;
        } else if (identifier == 2) {
            xPosBlue = xPos;
            yPosBlue = yPos;
            psiBlue = psi;
        }
        circle(image, Point2f(x2p(xPosActual), y2p(yPosActual)), 1, Scalar(0,0,255), 3);
        circle(image, Point2f(x2p(xPosRef), y2p(yPosRef)), 1, Scalar(0,255,0), 3);
        circle(image, Point2f(x2p(xPosBlue), y2p(yPosBlue)), 1, Scalar(255,0,0), 3);
        cout << "\nidentifier: \t" << identifier;
        cout << "\ntimestep:   \t" << timestep;
        cout << "\nxPos:       \t" << xPos;
        cout << "\nyPos:       \t" << yPos;
        cout << "\npsi:        \t" << psi;
        imshow("display", image);
        waitKey(1);

    }
    return 0;
}

// converts x position to pixel value
//_____________________________________________________________________________
float x2p(float x) {
    return (x*205.0)+265;
}

// converts y position to pixel value
//_____________________________________________________________________________
float y2p(float y) {
    return -(y*205.0)+100;
}
