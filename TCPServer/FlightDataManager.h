#pragma once
#include <mutex>

struct FlightData
{
	float pitch, bank, rudder, transX, transY;
	FlightData(float pitchInput, float bankInput, float yawInput, float transXInput, float transYInput)
	{
		this->pitch = pitchInput;
		this->bank = bankInput;
		this->rudder = yawInput;
		this->transX = transXInput;
		this->transY = transYInput;
	}

	//This default constructor is needed for some reasons I haven't looked into it.
	FlightData() {}
};


class FlightDataWrapper
{
public:
	std::mutex& GetMutex()
	{
		return m;
	}

	FlightData& GetFlightData()
	{
		std::lock_guard<std::mutex> guard(m);
		return this->currentFlightData;
	}

	void SetCurrentFlightData(FlightData& other)
	{
		std::lock_guard<std::mutex> guard(m);
		this->currentFlightData = other;
	}

	void Reset()
	{
		std::lock_guard<std::mutex> guard(m);
		this->currentFlightData = FlightData(0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	}
private:
	FlightData currentFlightData = FlightData(0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	std::mutex m;
};