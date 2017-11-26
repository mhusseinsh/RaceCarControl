// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../include/CameraCalibration.h"
#include "../include/CameraHandler.h"
#include "../include/Car.h"
#include "../include/Common.h"
#include "../include/UDPclient.h"

bool running;
bool resetAngles;

void signalHandler(int signal);

//_____________________________________________________________________________
int main( int argc, const char** argv ) {

    signal(SIGINT, signalHandler);

    CameraCalibration calibration;
    CameraHandler camera;
    std::vector<Car*> cars;

    camera.init();
    cv::Mat image = camera.getImage();
    cv::imwrite("background.tif", image);
    cv::Mat remerged;
    while(!calibration.averageImage(camera.getImage(), 100));
    calibration.createWindows();
    calibration.fitTrack();
    std::cout << "\npress ENTER to continue...";
    std::cin.get();
    calibration.destroyWindows();

    running = true;
    cv::namedWindow("original", 1);
    cv::namedWindow("bgsub", 1);
    cv::namedWindow("blue", 1);
    cv::namedWindow("green", 1);
    cv::namedWindow("red", 1);
    cv::namedWindow("blue*2", 1);
    cv::namedWindow("remerged", 1);
    while (running) {
        cv::Mat img = camera.getImage();
        image = img.clone();
        cv::imshow("original", img);
        image -= calibration._trackbackground;
        cv::threshold(image, image, 1.0, 0.0, cv::THRESH_TOZERO);
        image &= calibration._trackmask;
        cv::Mat channels[4];
        cv::split(image, channels);
        cv::imshow("bgsub", image);
        cv::imshow("blue", channels[0]);
        cv::imshow("green", channels[1]);
        cv::imshow("red", channels[2]);

        cv::imwrite("blue.tif", channels[0]);
        channels[0]*=2;
        cv::merge(channels, 4, remerged);
        cv::imshow("remerged", remerged);
        cv::imshow("blue*2", channels[0]);

        cv::imwrite("bgsub.tif", image);
        cv::imwrite("green.tif", channels[1]);
        cv::imwrite("red.tif", channels[2]);
        cv::imwrite("blue2.tif", channels[0]);
        cv::imwrite("remerged.tif", remerged);
        cv::imwrite("original.tif", img);

        if(cv::waitKey(33) != -1) exit(0);
    }
    exit (0);

    cars.clear();
    if (true) {
        Car* redCar = new Car();
        redCar->initialize("red", &calibration, 9930, 70);
        cars.push_back(redCar);
    }
    if (true) {
        Car* blueCar = new Car();
        blueCar->initialize("blue", &calibration, 9931, 70);
        cars.push_back(blueCar);
    }

    running = true;
    resetAngles = false;

    while(running) {

        std::stringstream timingInfo;
        long starttime = microtime();
        long currenttime = starttime;

        image = camera.getImage();
        image -= calibration._trackbackground;
        image &= calibration._trackmask;

        timingInfo << "\n[microseconds] Image acquisition: " << microtime() - currenttime;
        currenttime = microtime();

        for (auto& car : cars) {
            if (resetAngles) { car->resetAngles(); resetAngles = false; }
            car->update(image);

            timingInfo << "\n[microseconds] Detection of " << car->name << ":  " << microtime() - currenttime;
            currenttime = microtime();

            car->printData();
        }

        while(microtime() - starttime < PERIOD);
        timingInfo << "\n[microseconds] loop fill:   \t" << microtime() - currenttime;
        timingInfo << "\n[microseconds] loop time:   \t" << microtime() - starttime;

        std::cout << timingInfo.str();
    }
}



//_____________________________________________________________________________
// This function catches the "Ctrl+C" Keypress and cleanly exits after telling
// the control process to stop the motors and shutting down the camera.
void signalHandler(int signal){
    std::cout << "\n\nCaught Ctrl+C (signal %d)\n\n";
    std::cout << "q: quit\n";
    std::cout << "r: reset angles\n";
    char key = getchar();
    switch(key) {
    case 'q':
        running = false;
        break;
    case 'r':
        resetAngles = true;
        break;
    }
}
