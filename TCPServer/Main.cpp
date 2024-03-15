#include "Main.h"
#include "MSFSInputStrategy.h"
#include "ManualInputStrategy.h"
#include <unordered_map>
#include <typeindex>

#pragma comment (lib, "ws2_32.lib")

//Description in header file. We assign it here.
bool Main::running = false;

int main()
{
	//int main function of the program, creates a Main class and runs the execute function.
	Main m;
	m.execute();
	return 0;
}

void Main::execute()
{
	//wait until connected to PLC
	cout << "Connecting to PLC..." << endl;

	//Connect to the PLC, will wait until it does so.
	connector.connectToPLC();

	//Once connected, start up a second thread that constantly reads and writes data to and from the PLC.
	//It needs to do this so that both TCP buffers on the PC and PLC stay clean.
	//"Why use TCP instead of UDP, you fucking moron?" 
	//Well because my stupid ass for some reason couldn't get UDP to even work a little bit in CodeSys with the PLC i was allowed to use.
	//Could give that another try of course. It would be a much cleaner approach, maybe it was magically fixed in the weeks i was implementing a proper TCP setup for it. 
	//But I'm on a deadline here!
	std::thread tcpWorker(&Main::update, this);

	//Declare the input strategy we're going to let the user choose.
	std::unique_ptr<InputStrategy> inputStrategy;

	cout << "Select available inputs:\n" << "1. Manual Input\n" << "2. MSFS" << endl;


	std::unordered_map<std::string, std::unique_ptr<InputStrategy>> strategies;
	strategies.insert(std::make_pair<std::string, std::unique_ptr<InputStrategy>>("1", std::make_unique<ManualInputStrategy>()));
	strategies.insert(std::make_pair<std::string, std::unique_ptr<InputStrategy>>("2", std::make_unique<MSFSInputStrategy>()));

	//Set the static running to true.
	running = true;

	//Loop until proper input is given.
	while (true)
	{
		string input;
		cin >> input;

		//This if statement assigns the input strategy selected be the user. 
		//It's a pointer because I am stupid and don't have an idea how you can assign pure virtual functions on the stack.
		//It shouldn't cause a memory leak but I delete it at the end of the program anyways, for my mental state.
		try {
			inputStrategy = std::move(strategies[input]);
		}
		catch (exception e) {
			cout << "Error: Invalid Input" << endl;
		}
	}

	while (running)
	{
		try
		{
			FlightData fd = inputStrategy->getInput();
			fdw.SetCurrentFlightData(fd);
		}
		catch (std::exception e)
		{
			//If an exception the thrown by the getInput function the program will shut down.
			//I added a sleep to the thread so the other can assign any final values to the PLC.
			fdw.Reset();
			std::this_thread::sleep_for(chrono::seconds(1));
			running = false;
		}
	}

	//Rejoin tcpWorker and close the connection to the PLC.
	tcpWorker.join();
	connector.closeConnection();
}

void Main::update()
{
	float* currentPositions = NULL;
	Calculator::CalculationData calcData {};
	while (running)
	{	
		try 
		{
			currentPositions = connector.getCurrentPositions();
			calcData = this->calculate(currentPositions);
			this->send(calcData);
		}
		catch (std::exception e) 
		{
			continue;
		}
	}
}

Calculator::CalculationData Main::calculate(float* currentPositions) throw (std::exception)
{ 
	//Grab the current lengths of the pistons from the PLC.
	//With this array pointer the calculator can calculate the delta velocities needed for the pistons to move and stop at the same time.

	//Run the calculations using a reference to the currentFlightData object and the pointer to the currentPositions array.
	//Lock data in struct
	std::lock_guard<std::mutex> guard(fdw.GetMutex());
	return calculator.run(fdw.GetFlightData(), currentPositions);
}

void Main::send(Calculator::CalculationData calculationResults)
{
	nlohmann::json j;
	j["positions"] = calculationResults.lengths;
	j["speeds"] = calculationResults.speeds;

	connector.sendMessage(j.dump());
}










