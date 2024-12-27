#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>


int main()
{
	WSADATA wsaData; 
	int iResult; 
	
	// initialize wsadata
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	// creates socket
	SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// setup tcp listening socket
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr("127.0.0.1");
	service.sin_port = htons(80);

	// bind to service
	bind(ListenSocket, (SOCKADDR*)&service, sizeof(service));

	// listen
	listen(ListenSocket, SOMAXCONN);

	std::cout << "Waiting for clients to connect...\n";

	SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);

	std::cout << "Client connected...\n";


}