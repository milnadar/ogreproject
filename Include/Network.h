#ifndef NETWORK_H
#define NETWORK_H

#include <winsock2.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>

// Need to link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

SOCKET server_socket = INVALID_SOCKET;

SOCKET client_socket_in = INVALID_SOCKET;
SOCKET client_socket_out = INVALID_SOCKET;

SOCKET server_socket_in = INVALID_SOCKET;
SOCKET server_socket_out = INVALID_SOCKET;

bool NETWORK_CLIENT_CONNECTED = false;
bool NETWORK_SERVER_CONNECTED = false;

bool serverIsRunning = false;
const short int serverPort = 27015;

DWORD WINAPI startClientLoop(LPVOID socket)
{
	std::cout << "Client loop started\n";
	int num_bytes = SOCKET_ERROR;
	char data[256];
	do
	{
		num_bytes = recv(client_socket_in, data, sizeof(data), 0);
		std::cout << "received data from server\n";
	} while(num_bytes > 0);
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
	return 0;
}

DWORD WINAPI startServerLoop(LPVOID socket)
{
	std::cout << "Server loop started\n";
	int num_bytes = SOCKET_ERROR;
	char data[256];
	do
	{
		num_bytes = recv(server_socket_in, data, sizeof(data), 0);
		std::cout << "received data from client\n";
	} while(num_bytes > 0);
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
	NETWORK_CLIENT_CONNECTED = false;
}

void Network::shutDown()
{
	stopServer();
	closeClient();
	WSACleanup();
}

#endif