// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

#ifndef BOX_H_
#define BOX_H_

#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "../include/Common.h"


class Box {

public:
  Box();
  Box(double boxX, double boxY, double boxAngle, double boxWidth, double boxHeight, Color color);

  void initialize(cv::RotatedRect* rRect, cv::Mat channels[4]);
  Color checkColor();
  void print();

  double x;
  double y;
  double angle;
  double width;
  double height;

  Color color;

  bool dataValid;

private:

};

#endif // BOX_H

