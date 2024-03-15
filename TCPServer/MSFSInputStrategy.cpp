#include "MSFSInputStrategy.h"

MSFSInputStrategy::MSFSInputStrategy() 
{
	//This constructor will check if MSFS is opened and loaded. If not, program should shut down.
	//To make this nicer you could make this a loop that waits for the game to open and can be cancelled.
	if (SUCCEEDED(SimConnect_Open(&simConnect, "Client Event", NULL, NULL, NULL, NULL)))
	{
		std::cout << "Connected to MSFS..." << std::endl;

		//Select which variables are requested by the dispatcher. The order should be the same as the order in which the variables in the output object are declared.
		//So in the FlightData struct pitch is declared first, next bank and finally rudder position.
		//TranslationX and TranslationY will have to be filled out with other variables from SimConnect. What these variables are is up for the next group to decide ;)
		hResult = SimConnect_AddToDataDefinition(simConnect, DEF1, "PLANE PITCH DEGREES", "degrees", SIMCONNECT_DATATYPE_FLOAT32);
		hResult = SimConnect_AddToDataDefinition(simConnect, DEF1, "PLANE BANK DEGREES", "degrees", SIMCONNECT_DATATYPE_FLOAT32);
		hResult = SimConnect_AddToDataDefinition(simConnect, DEF1, "RUDDER POSITION", "degrees", SIMCONNECT_DATATYPE_FLOAT32);

		//Here is the data request, the last variable is the refresh rate of the data. 
		//I think the fastest is the Sim frame, but feel free to experiment with a framerate based refresh rate
		hResult = SimConnect_RequestDataOnSimObject(simConnect, REQ1, DEF1, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SIM_FRAME);
	}
	else
	{
		std::cout << "Failed to connect to MSFS..." << std::endl;
	}
}

FlightData MSFSInputStrategy::getInput()
{
	//Below is the pointer is declared (pData) to which the data of the request will be written. As well as the size of the object (cbData)
	SIMCONNECT_RECV* pData;
	DWORD cbData;

	//GetNextDispatch will get the next data packet from simConnect, both the data and size pointers are sent here as a reference to be filled out.
	hResult = SimConnect_GetNextDispatch(simConnect, &pData, &cbData);

	//Check if the dispatch successfully returned a result. 
	//It is the case that a new loaded game will fail a couple of requests, this should not affect the movement of the platform, so nothing is done.
	if (SUCCEEDED(hResult))
	{
		//This switch will check for a few dataIDs received from the dispatch.
		//If the ID returned is not relevant to the operation of the platform, nothing is done.
		//If the ID returned matches simulation object data, it will enter the second case.
		//If the ID returned matches a game shut down request, it will enter the third case.
		switch (pData->dwID)
		{
		default:
			break;

		case SIMCONNECT_RECV_ID_SIMOBJECT_DATA:
		{
			//Cast pData to a simobject data object pointer.
			SIMCONNECT_RECV_SIMOBJECT_DATA* objectData = (SIMCONNECT_RECV_SIMOBJECT_DATA*)pData;

			//Check if objectData request ID matches the REQ1 ID enum defined the header file.
			//Should this not match nothing will happen.
			if (objectData->dwRequestID == REQ1)
			{
				//Cast simobject data to FlightData object.
				Calculator::FlightData fd = *(Calculator::FlightData*)&objectData->dwData;

				//Due to the configuration of the platform, bank and rudder should be inversed here.
				fd.bank = -fd.bank;
				fd.rudder = -fd.rudder;

				//Limit pitch, bank and yaw for testing. Also disable translation movements for now
				fd.transX = 0.0f;
				fd.transY = 0.0f;

				cout << "\rBank: " << fd.bank << ", Pitch: " << fd.pitch << flush;

				if (fd.bank > outputLimit)
				{
					fd.bank = outputLimit;
				}
				else if (fd.bank < -outputLimit)
				{
					fd.bank = -outputLimit;
				}

				if (fd.pitch > outputLimit)
				{
					fd.pitch = outputLimit;
				}
				else if (fd.pitch < -outputLimit)
				{
					fd.pitch = -outputLimit;
				}

				if (fd.rudder > outputLimit)
				{
					fd.rudder = outputLimit;
				}
				else if (fd.rudder < -outputLimit)
				{
					fd.rudder = -outputLimit;
				}

				return fd;
			}
			break;
		}

		case SIMCONNECT_RECV_ID_QUIT:
		{
			//throw exception if game is shut off;
			throw std::exception();
			break;
		}

		}
	}
}