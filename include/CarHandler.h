// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

#ifndef CARHANDLER_H_
#define CARHANDLER_H_

#include <opencv2/core/core.hpp>
#include <map>
#include <vector>
#include <string>

#include "Box.h"
#include "Car.h"
#include "Common.h"

class CarHandler {

public:
	CarHandler();
	~CarHandler();
	void initCar(Color carColor, int portNumber);
	void update(std::map<Color,Box> carBoxes, std::string obstacleString);
	void resetAngles();
	void closeOutputFiles();

	std::map<Color,Car*> cars;

private:
};

#endif // CARHANDLER_H_
