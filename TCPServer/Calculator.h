#include "Math.h"
#include <Mutex>
#include "FlightDataManager.h"

#pragma once
class Calculator
{
private:
	//Look into the final report if you want to know more about how we came about the values in these variables.
	float pi = 2 * acos(0);
	float pistonLengths[6];
	float pistonSpeeds[6]{ 0,0,0,0,0,0 };

	float bankRadian, pitchRadian, yawRadian;
	float cosinRoll, cosinPitch, cosinYaw, sinRoll, sinPitch, sinYaw;

	float legLength = 824.0f;
	float translationX = 0;
	float translationY = 0;
	float translationZ = 935.7456f;

	float defaultXPosition[6]{ 537.69f, 715.23f, 177.53f, -177.53f, -715.25f, -537.69 };
	float defaultYPosition[6]{ 515.43f, 207.94, -723.37f, -723.37f, 207.94f, 515.43f, };

	float defaultXBasePosition[6]{ 177.53f, 715.23f, 537.69f, -537.69f, -715.23f, -177.53f, };
	float defaultYBasePosition[6]{ 723.37, -207.94, -515.43f, -515.43f, -207.94f, 723.37f };

	float positionsX[6], positionsY[6], positionsZ[6];

	float pistonMovementTime;

	//Max velocity is currently 200 due to platform overshoot. The true max velocity is 400.
	//This should be changed once the overshoot problem is fixed
	float maxVelocity = 200;
	float pistonDeltaLengths[6];
public:
	struct CalculationData 
	{
		float lengths[6], speeds[6];
	};

	CalculationData run(FlightData& fd, float* oldPistonlengths);
};

