#include "ManualInputStrategy.h"

FlightData ManualInputStrategy::getInput()
{
	//Define variables needed for manual input. 
	//Wait with assigning these variable until all of them are filled, otherwise you'll get wonky results.
	float bankInput, pitchInput, yawInput, transXInput, transYInput;
	cout << "Bank: " << endl;

	cin >> bankInput;

	cout << "Pitch: " << endl;;

	cin >> pitchInput;

	cout << "Yaw: " << endl;;

	cin >> yawInput;

	cout << "TransX: " << endl;;

	cin >> transXInput;

	cout << "TransY: " << endl;;

	cin >> transYInput;


	FlightData fd(pitchInput, bankInput, yawInput, transXInput, transYInput);
	return fd;
}
