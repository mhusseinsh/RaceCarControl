// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

#ifndef COMMON_H_
#define COMMON_H_

#include <string>

// This header contains common definitions and functions.

// display camera feed and binary after threshold (for debugging or tuning reasons)
//#define DISPLAYLIVE

 // microseconds (equals 1/framerate)
#define PERIOD 10000.0

// track to be parsed
#define TRACK "tracks/Ref_LMS_Final_v0p5.txt"

// Returns timestamp (real time clock) in microseconds.
// To be used in a relative manner (e.g.: timeB - timeA).
long microtime();

void timestamp(double *y);

void MouseCallBack(int event, int x, int y, int flags, void* obj);

extern int mouseX;
extern int mouseY;
extern bool calibrationDone;
extern bool cnnDatasetMode;
extern int cnnCounter;
extern bool cnnCapture;

extern double psiAfterBoxDetection;

enum class Color {
  INVALID = -1,
  BLUE = 0,
  GREEN = 1,
  RED = 2,
  WHITE = 3,
  YELLOW = 4,
  OBSTACLECOLORED = 5
};

std::string colorToString(Color color);

#endif // COMMON_H_
