#pragma once
#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <nlohmann/json.hpp>


using namespace std;

class TCPConnector
{
private:
	SOCKET clientSocket;
	char buf[4096];
	int bufSize = 4096;
	float currentPositions[6];
public:
	void connectToPLC();
	void sendMessage(string msg);
	void closeConnection();
	float* getCurrentPositions();
};

