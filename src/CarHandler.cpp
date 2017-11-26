// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

#include "../include/CarHandler.h"

#include <opencv2/core/core.hpp>

#include <utility>
#include <iostream>
#include <sstream>

#include "../include/Car.h"

CarHandler::CarHandler() {
}

CarHandler::~CarHandler() {
	for (auto& pair : cars) {
		delete pair.second;
	}
}

void CarHandler::initCar(Color carColor, int portNumber) {
	Car* car = new Car();
	cars[carColor] = car;
	cars[carColor]->initialize(carColor, portNumber);
}


void CarHandler::update(std::map<Color,Box> carBoxes, std::string obstacleString) {
	std::cout << "\nobstacleString: " << obstacleString;
	for (auto& pair : cars) {
		pair.second->update(carBoxes[pair.second->color], obstacleString);
	}
}

void CarHandler::resetAngles() {
	for (auto& pair : cars) {
		pair.second->resetAngle();
	}
}

void CarHandler::closeOutputFiles() {
	for (auto& pair : cars) {
		pair.second->dataout.close();
	}
}
