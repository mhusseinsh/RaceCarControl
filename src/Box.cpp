// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "../include/Box.h"
#include "../include/Common.h"

Box::Box() {
	x = 0.0;
	y = 0.0;
	angle = 0.0;
	width = 0.0;
	height = 0.0;
	color = Color::INVALID;
	dataValid = false;
}

Box::Box(double boxX, double boxY, double boxAngle, double boxWidth, double boxHeight, Color boxColor) {
	x = boxX;
	y = boxY;
	angle = boxAngle;
	width = boxWidth;
	height = boxHeight;
	color = boxColor;
	dataValid = true;
}

void Box::print() {
	std::cout << "\n\nColor:  " << colorToString(color);
	std::cout << "\nx:      " << x ;
	std::cout << "\ny:      " << y ;
	std::cout << "\nangle:  " << angle ;
	std::cout << "\nheight: " << height ;
	std::cout << "\nwidth:  " << width ;
	std::cout << "\n\n";
}
