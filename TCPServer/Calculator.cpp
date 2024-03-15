#include "Calculator.h"

Calculator::CalculationData Calculator::run(FlightData& fd, float* oldPistonLengths) 
{
	//We convert the degree values we received from the FlightData object into Radians.
	bankRadian = fd.bank * pi / 180;
	pitchRadian = fd.pitch * pi / 180;
	yawRadian = fd.rudder * pi / 180;

	//Get the cos and sin values of these Radians.
	//I'm nowhere near good enough at math to explain it to you. So just look into the final report we left.
	cosinRoll = cos(bankRadian);
	cosinPitch = cos(pitchRadian);
	cosinYaw = cos(yawRadian);

	sinRoll = sin(bankRadian);;
	sinPitch = sin(pitchRadian);
	sinYaw = sin(yawRadian);

	float res, nearestRound;
	CalculationData calcData;

	for (int i = 0; i < 6; i++) {
		//Calculation the positions of something i forgot.
		positionsX[i] = fd.transX + cosinYaw * cosinRoll * defaultXPosition[i] + (-sinYaw * cosinPitch + cosinYaw * sinRoll * sinPitch) * defaultYPosition[i];
		positionsY[i] = fd.transY + sinYaw * cosinRoll * defaultXPosition[i] + defaultYPosition[i] * (cosinYaw * cosinPitch + sinYaw * sinPitch * sinRoll);
		positionsZ[i] = translationZ - sinRoll * defaultXPosition[i] + cosinRoll * sinPitch * defaultYPosition[i];

		//The result is the length one of the pistons. We round it to 2 decimals because the json string would get massive.
		res = sqrt(pow(positionsX[i] - defaultXBasePosition[i], 2) + pow(positionsY[i] - defaultYBasePosition[i], 2) + pow(positionsZ[i], 2)) - legLength;
		nearestRound = roundf(res * 100) / 100;

		//If one of the pistons is detected to be longer than 400 or shorther than 5 an exception is throw and nothing is done.
		//This is to prevent the pistons from going into error or breaking themselves.
		//This is a very simple and VERY bad solution. It requires some other mathmetical formula to make sure the length stay within the proper range.
		//Please look into that btw, the operation is kinda bad because of it.
		if (nearestRound > 400.0f || nearestRound < 5.0f)
		{
			throw std::exception();
		}

		calcData.lengths[i] = nearestRound;
	}

	//Set the max piston delta length to 0 to start with, this will be assigned in the following loop to calculate proper velocities.
	float maxPistonDeltaLength = 0.0f;

	for (int i = 0; i < 6; i++)
	{
		//Old length minus new length, set it to the absolute value.
		pistonDeltaLengths[i] = oldPistonLengths[i] - calcData.lengths[i];
		pistonDeltaLengths[i] = abs(pistonDeltaLengths[i]);

		//Compare current pistonDeltaLength with the max, replace if higher.
		if (pistonDeltaLengths[i] > maxPistonDeltaLength)
		{
			maxPistonDeltaLength = pistonDeltaLengths[i];
		}

		//Get piston Movement time.
		pistonMovementTime = maxPistonDeltaLength / maxVelocity;
	}

	for (int i = 0; i < 6; i++)
	{
		//The result is the speed needed for each piston to stop moving at the same time.
		res = pistonDeltaLengths[i] / pistonMovementTime;
		nearestRound = roundf(res * 100) / 100;
		calcData.speeds[i] = nearestRound;
	}

	return calcData;
}
