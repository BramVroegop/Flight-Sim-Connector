#pragma once
#include <iostream>
#include "TCPConnector.h"
#include <string>
#include <thread>
#include <nlohmann/json.hpp>
#include "Calculator.h"
#include "FlightDataManager.h"

class Main {
private:
	TCPConnector connector;
	Calculator calculator;
	FlightDataWrapper fdw;
	
	void update();
	Calculator::CalculationData calculate(float* currentPositions);
	void send(Calculator::CalculationData calculationResults);
public:
	void execute();

	//Static boolean to check if the application should continue, can be accessed from anywhere.
	static bool running;	
};

