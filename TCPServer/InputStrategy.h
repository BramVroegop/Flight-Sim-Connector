#pragma once
#include "Main.h"
class InputStrategy
{
public:
	//Pure virtual function that can be implemented as long as it returns a valid FlightData object. 
	virtual FlightData getInput() = 0;
};

