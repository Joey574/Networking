#include <winsock2.h> 
#include <ws2tcpip.h> 
#include <iostream>
#include <string>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

#define INC_PORT 27015
#define INC_IP "127.0.0.1"

#define OUT_PORT 28015
#define OUT_IP "127.0.0.1"

#define LOG 1

SOCKET setup_listen_server() {
	sockaddr_in server;

	server.sin_family = AF_INET;
	server.sin_port = htons(INC_PORT);
	inet_pton(AF_INET, INC_IP, &server.sin_addr);

	SOCKET connection = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	int err = connect(connection, (struct sockaddr*)&server, sizeof(server));
	if (err) {
		std::cout << "CONNECTION ERROR\n";
		return -1;
	}

	return connection;
}

SOCKET setup_send_server() {

}

int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	std::vector<std::string> cmds();

	WSACleanup();
}