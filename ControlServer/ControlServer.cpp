#include <winsock2.h> 
#include <ws2tcpip.h> 
#include <iostream>
#include <conio.h>
#include <string>

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

static enum cmdByte {
	keep_alive = '\x01',
	kill_conn = '\x02'
};

int send_command_to_server(const SOCKET& connection, std::string msg) {
	#if LOG
	std::cout << "Sending '" << msg << "' to server\n";
	#endif

	int err = send(connection, &msg[0], msg.length(), 0);

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

void init() {
	std::cout << "Control Server:\n";
}

int main() {
	init();

	WSADATA wsaData;
	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);

	sockaddr_in server;
	SOCKET ListenSocket;

	err = setup_server(server, ListenSocket);
	if (err) {
		std::cout << "Error setting up server\n";
		_getch();
		return 1;
	}

	// listen and accept incoming connection
	#if LOG
	std::cout << "Listening for connection\n";
	#endif

	listen(ListenSocket, SOMAXCONN);
	SOCKET accepted = accept(ListenSocket, NULL, NULL);

	std::string msg;

	msg = "echo hello world!";
	msg.push_back((char)keep_alive);
	err = send_command_to_server(accepted, msg);

	msg = "echo second message";
	msg.push_back((char)keep_alive);
	err = send_command_to_server(accepted, msg);

	msg = "systeminfo";
	msg.push_back((char)kill_conn);
	err = send_command_to_server(accepted, msg);

	closesocket(accepted);

	if (err) {
		std::cout << "Error sending command to server\n";
		_getch();
		return 1;
	}


	WSACleanup();
	_getch();
}