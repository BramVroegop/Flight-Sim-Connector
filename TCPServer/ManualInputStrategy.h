#pragma once
#include "InputStrategy.h"

class ManualInputStrategy : public InputStrategy
{
public:
	FlightData getInput() override;
};

