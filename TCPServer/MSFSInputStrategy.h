#pragma once
#include "InputStrategy.h"
#include "SimConnect.h"

class MSFSInputStrategy : public InputStrategy
{
private:
	static enum DATA_DEFINE_ID
	{
		//Data definition assigned to hResult.
		DEF1
	};

	static enum DATA_REQUEST_ID
	{
		//Required to match request ID upon receiving data from the dispatcher.
		REQ1
	};

	//Main simconnect interaction object.
	HANDLE simConnect = NULL;

	//Main simconnect Request object.
	HRESULT hResult;

	//Limiter Float for msfs ouput
	float outputLimit = 7.0f;
public:
	MSFSInputStrategy();
	FlightData getInput() override;
};

