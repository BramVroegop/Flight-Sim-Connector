#include "TCPConnector.h"
#include "Main.h"

using namespace std;

float* TCPConnector::getCurrentPositions()
{	
	//Clear memory of the buffer.
	ZeroMemory(buf, bufSize);

	// Wait for client to send data
	int bytesReceived = _WINSOCK2API_::recv(clientSocket, buf, bufSize, 0);
	
	//If client does not send data or disconnects the program will shut down.
	if (bytesReceived == 0 || bytesReceived == SOCKET_ERROR)
	{
		cout << "Client disconnected" << endl;
		Main::running = false;
	}

	nlohmann::json data;

	//Try to parse data, throws exception if failed, catch exception in sendPositions function
	data = nlohmann::json::parse(std::string(buf, 0, bytesReceived));
	auto lengths = data["currentPositions"];

	int currentPos = 0;
	for (auto it = lengths.begin(); it != lengths.end(); ++it)
	{
		currentPositions[currentPos] = it.value();
		currentPos++;
	}

	//Return pointer of the first value of the array.
	return currentPositions;
}

void TCPConnector::connectToPLC()
{
	// Initialze winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = _WINSOCK2API_::WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		cerr << "Can't Initialize winsock! Quitting" << endl;
		return;
	}

	// Create a socket
	SOCKET listening = _WINSOCK2API_::socket(AF_INET, SOCK_STREAM, 0);

	if (listening == INVALID_SOCKET)
	{
		cerr << "Can't create a socket! Quitting" << endl;
		return;
	}

	// Bind the ip address and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(32760);
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could also use inet_pton .... 

	//Make sure to use the winsock2 namespace bc holy fuck I wasted an entire day on figuring out what was wrong
	_WINSOCK2API_::bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell Winsock the socket is for listening 
	_WINSOCK2API_::listen(listening, SOMAXCONN);

	// Wait for a connection
	sockaddr_in client;
	int clientSize = sizeof(client);

	clientSocket = _WINSOCK2API_::accept(listening, (sockaddr*)&client, &clientSize);

	char host[NI_MAXHOST];		// Client's remote name
	char service[NI_MAXSERV];	// Service (i.e. port) the client is connect on

	ZeroMemory(host, NI_MAXHOST); // same as memset(host, 0, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << host << " connected on port " << service << endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port " << ntohs(client.sin_port) << endl;
	}

	Main::running = true;

	// Close listening socket
	closesocket(listening);
}

void TCPConnector::sendMessage(string msg) 
{
	//Sends message to the client socket, remember the message size should always have the size + 1.
	_WINSOCK2API_::send(clientSocket, msg.c_str(), msg.size() + 1, 0);
}

void TCPConnector::closeConnection() 
{
	cout << "Finished..." << endl;

	// Close the socket
	_WINSOCK2API_::closesocket(clientSocket);

	// Cleanup winsock
	_WINSOCK2API_::WSACleanup();
}
