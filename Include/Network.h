#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <winsock2.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <queue>

// Need to link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#define __FREE_QUEUE(ptr) while(!ptr.empty()) {char* data = ptr.front(); delete [] data; ptr.pop();};

SOCKET server_socket = INVALID_SOCKET;

SOCKET client_socket_in = INVALID_SOCKET;
SOCKET client_socket_out = INVALID_SOCKET;
bool NETWORK_CLIENT_CONNECTED = false;
std::queue<char*>NETWORK_CLIENT_QUEUE;

SOCKET server_socket_in = INVALID_SOCKET;
SOCKET server_socket_out = INVALID_SOCKET;
bool NETWORK_SERVER_CONNECTED = false;
std::queue<char*>NETWORK_SERVER_QUEUE;

bool serverIsRunning = false;
const short int serverPort = 27015;

DWORD WINAPI startClientLoop(LPVOID socket)
{
	std::cout << "Client loop started\n";
	int num_bytes = SOCKET_ERROR;
	char buffer[256];
	do
	{
		num_bytes = recv(client_socket_in, buffer, sizeof(buffer), 0);
		if(num_bytes > 0)
		{
			if(buffer[0] == 0)
			{
				//echo
			}
			else
			{
				char *data = new char[num_bytes + 1];
				data[0] = (char)num_bytes;
				::memcpy((LPVOID)&data[1], (LPVOID)buffer, num_bytes);
				NETWORK_CLIENT_QUEUE.push(data);
			}
		}
		else break;
	}
	while(num_bytes > 0);
	std::cout << "Connection with server has been lost\n";
	NETWORK_CLIENT_CONNECTED = false;
	if(client_socket_in != INVALID_SOCKET)
	{
		closesocket(client_socket_in);
		client_socket_in = INVALID_SOCKET;
	}
	if(client_socket_out != INVALID_SOCKET)
	{
		closesocket(client_socket_in);
		client_socket_out = INVALID_SOCKET;
	}
	__FREE_QUEUE(NETWORK_CLIENT_QUEUE);
	return 0;
}

DWORD WINAPI startServerLoop(LPVOID socket)
{
	std::cout << "Server loop started\n";
	int num_bytes = SOCKET_ERROR;
	char buffer[256];
	do
	{
		num_bytes = recv(server_socket_in, buffer, sizeof(buffer), 0);
		if(num_bytes > 0)
		{
			if(buffer[0] == 0)
			{
				//
			}
			else
			{
				char* data = new char[num_bytes + 1];
				data[0] = (char)num_bytes;
				::memcpy((LPVOID)&data[1], (LPVOID)buffer, num_bytes);
				NETWORK_SERVER_QUEUE.push(data);
			}
		}
		else break;
	}
	while(num_bytes > 0);
	std::cout << "Connection with client has been lost\n";
	NETWORK_SERVER_CONNECTED = false;
	if(server_socket_in != INVALID_SOCKET)
	{
		closesocket(server_socket_in);
		server_socket_in = INVALID_SOCKET;
	}
	if(server_socket_out != INVALID_SOCKET)
	{
		closesocket(server_socket_in);
		server_socket_out = INVALID_SOCKET;
	}
	__FREE_QUEUE(NETWORK_SERVER_QUEUE);
	return 0;
}

class Network
{
public:
	Network();
	~Network();
	bool initialiseNetwork();
	bool createServer();
	bool connectToServer(char *stringAddress);
	void stopServer();
	void closeClient();
	int getDataFromServer(char *data, int bufferSize);
	int getDataFromClient(char *data, int bufferSize);
	bool sendDataToServer(char *data, int dataSize = -1);
	bool sendDataToClient(char *data, int dataSize = -1);
	void shutDown();
private:
	SOCKET listen_socket;
};

Network::Network()
{
	listen_socket = INVALID_SOCKET;
}

Network::~Network()
{
	shutDown();
}

bool Network::initialiseNetwork()
{
	if(server_socket != SOCKET_ERROR)
		return false;
	WSAData wsadata;
	int result;
	result = WSAStartup(MAKEWORD(2,2), &wsadata);
	if(result != 0)
	{
		std::cout << "Startup failed";
		return false;
	}
	return true;
}

bool Network::createServer()
{
	int result;
	if(server_socket != INVALID_SOCKET)
		return true;
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(server_socket == INVALID_SOCKET)
	{
		std::cout << "socket creation failed";
		WSACleanup();
		return false;
	}
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(serverPort);
	result = bind(server_socket, (SOCKADDR*)&server_addr, sizeof(server_addr));
	if(result == SOCKET_ERROR)
	{
		std::cout << "binding failed";
		closesocket(server_socket);
		WSACleanup();
		return false;
	}
	result = listen(server_socket, 2);
	if(result == SOCKET_ERROR)
	{
		std::cout << "listen failed";
		closesocket(server_socket);
		WSACleanup();
		return false;
	}
	sockaddr_in client_addr;
	int client_addr_size = sizeof(client_addr);
	std::cout << "Waiting for clients...\n";
	SOCKET client;
	while (client = accept(server_socket, (SOCKADDR*)&client_addr, &client_addr_size))
	{
		if(server_socket_in == INVALID_SOCKET)
		{
			std::cout << "client connected to server in\n";
			server_socket_in = client;
			DWORD threadID;
			CreateThread(NULL, NULL, startServerLoop, NULL, NULL, &threadID);
		}
		else if(server_socket_out == INVALID_SOCKET)
		{
			std::cout << "client connected to server out\n";
			server_socket_out = client;
			closesocket(server_socket);
			server_socket = INVALID_SOCKET;
			NETWORK_SERVER_CONNECTED = true;
			return true;
		}
		else
		{
			//enough connections, so stop listening
			closesocket(server_socket);
			server_socket = INVALID_SOCKET;
			break;
		}
	}
	return false;
}

bool Network::connectToServer(char* address)
{
	closeClient();
	int result;
	//open socket to send data
	client_socket_out = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(client_socket_out == INVALID_SOCKET)
	{
		std::cout << "failed to create clients socket\n";
		return false;
	}
	sockaddr_in client_addr;
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(serverPort);
	client_addr.sin_addr.s_addr = inet_addr(address);
	std::cout << "connecting to address " << address << '\n';
	result = connect(client_socket_out, (LPSOCKADDR)&client_addr, sizeof(client_addr));
	if(result == SOCKET_ERROR)
	{
		std::cout << "connection failed " << WSAGetLastError() << '\n';
		closesocket(client_socket_out);
		client_socket_out = INVALID_SOCKET;
		return false;
	}
	std::cout << "Connetion established for socket_out\n";

	//open socket to receive data
	client_socket_in = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(client_socket_in == INVALID_SOCKET)
	{
		std::cout << "failed to create clients socket\n";
		return false;
	}
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(serverPort);
	client_addr.sin_addr.s_addr = inet_addr(address);
	std::cout << "connecting to address " << address << '\n';
	result = connect(client_socket_in, (LPSOCKADDR)&client_addr, sizeof(client_addr));
	if(result == SOCKET_ERROR)
	{
		std::cout << "connection failed " << WSAGetLastError() << '\n';
		closesocket(client_socket_in);
		client_socket_in = INVALID_SOCKET;
		return false;
	}
	std::cout << "Connetion established for socket_in\n";
	DWORD threadID;
	CreateThread(NULL, NULL, startClientLoop, NULL, NULL, &threadID);
	NETWORK_CLIENT_CONNECTED = true;
	return true;
}

void Network::stopServer()
{
	if(server_socket != INVALID_SOCKET)
	{
		closesocket(server_socket);
		server_socket = INVALID_SOCKET;
	}
	if(server_socket_in != INVALID_SOCKET)
	{
		closesocket(server_socket_in);
		server_socket_in = INVALID_SOCKET;
	}
	if(server_socket_out != INVALID_SOCKET)
	{
		closesocket(server_socket_out);
		server_socket_out = INVALID_SOCKET;
	}
	__FREE_QUEUE(NETWORK_SERVER_QUEUE);
	NETWORK_SERVER_CONNECTED = false;
}

void Network::closeClient()
{
	if(client_socket_in != INVALID_SOCKET)
	{
		closesocket(client_socket_in);
		client_socket_in = INVALID_SOCKET;
	}
	if(client_socket_out != INVALID_SOCKET)
	{
		closesocket(client_socket_in);
		client_socket_out = INVALID_SOCKET;
	}
	__FREE_QUEUE(NETWORK_CLIENT_QUEUE);
	NETWORK_CLIENT_CONNECTED = false;
}

int Network::getDataFromClient(char *lpData, int bufferSize)
{
	if(NETWORK_SERVER_QUEUE.empty())
		return 0;
	char * data = NETWORK_SERVER_QUEUE.front();
	if(data[0] > bufferSize)
		return 0;
	::memcpy((LPVOID)lpData, (LPVOID)&data[1], data[0]);
	bufferSize = (int)data[0];
	NETWORK_SERVER_QUEUE.pop();
	delete [] data;
	return bufferSize;
}

int Network::getDataFromServer(char *lpData, int bufferSize)
{
	if(NETWORK_CLIENT_QUEUE.empty())
		return 0;
	char * data = NETWORK_CLIENT_QUEUE.front();
	if(bufferSize < data[0])
		return 0;
	::memcpy((LPVOID)lpData, (LPVOID)&data[1], data[0]);
	bufferSize = (int) data[0];
	NETWORK_CLIENT_QUEUE.pop();
	delete [] data;
	return bufferSize;
}

bool Network::sendDataToClient(char * lpData, int dataSize)
{
	if(server_socket_out == INVALID_SOCKET)
		return false;
	if(dataSize == -1)
		dataSize = (int) ::strlen(lpData) + 1;
	int result = send(server_socket_out, lpData, dataSize, 0);
	if(result == SOCKET_ERROR)
	{
		std::cout << "couldn't send data\n";
		return false;
	}
	return true;
}

bool Network::sendDataToServer(char * lpData, int dataSize)
{
	if(client_socket_out == INVALID_SOCKET)
		return false;
	if(dataSize == -1)
		dataSize = (int) ::strlen(lpData) + 1;
	int result = send(client_socket_out, lpData, dataSize, 0);
	if(result == SOCKET_ERROR)
	{
		std::cout << "couldn't send data\n";
		return false;
	}
	return true;
}

void Network::shutDown()
{
	stopServer();
	closeClient();
	WSACleanup();
}

#endif