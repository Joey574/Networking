#include <winsock2.h> 
#include <ws2tcpip.h> 
#include <iostream>
#include <conio.h>

#pragma comment(lib, "Ws2_32.lib")

#define BUF_LEN 512
#define DEF_PORT 27015
#define SERVER_IP "127.0.0.1"

#define LOG 1

/*
END BYTE COMMAND
\x01	Keep Alive
\x02	Kill Connection


*/

int send_command_to_server(const SOCKET& connection, const char* msg, size_t len) {
	#if LOG
	std::cout << "Sending '" << std::string(msg, len - 1) << "' to server\n";
	#endif

	int err = send(connection, msg, len, 0);

	if (err == SOCKET_ERROR) {
		return 1;
	}

	return 0;
}

int setup_server(sockaddr_in& server, SOCKET& ListenSocket) {
	server.sin_family = AF_INET;
	server.sin_port = htons(DEF_PORT);
	inet_pton(AF_INET, SERVER_IP, &server.sin_addr);

	ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int err = bind(ListenSocket, (SOCKADDR*)&server, sizeof(server));
	if (err) { return 1; }

	#if LOG
	std::cout << "Setup server on '" << SERVER_IP << "' on port '" << DEF_PORT << "'\n";
	#endif

	return 0;
}

int main() {
	std::cout << "Control Server:\n";

	WSADATA wsaData;
	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);

	sockaddr_in server;
	SOCKET ListenSocket;

	err = setup_server(server, ListenSocket);
	if (err) {
		std::cout << "Error setting up server\n";
		return 1;
	}

	// listen and accept incoming connection
	#if LOG
	std::cout << "Listening for connection\n";
	#endif

	listen(ListenSocket, SOMAXCONN);
	SOCKET accepted = accept(ListenSocket, NULL, NULL);

	const char* msgA = "echo hello world!\x01";
	const char* msgB = "echo second message\x01";
	const char* msgC = "systeminfo\x02";

	err = send_command_to_server(accepted, msgA, strlen(msgA));
	err = send_command_to_server(accepted, msgB, strlen(msgB));
	err = send_command_to_server(accepted, msgC, strlen(msgC));

	closesocket(accepted);

	if (err) {
		std::cout << "Error sending command to server\n";
		return 1;
	}


	WSACleanup();
	_getch();
}