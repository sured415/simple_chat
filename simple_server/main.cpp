#include <winsock2.h>					//socket
#include <Ws2tcpip.h>					// inet_pton
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <process.h>
#include <list>

using namespace std;

list<SOCKET> clients;

unsigned WINAPI client_chat(void* a) {
	SOCKET c = *(SOCKET*)a;
	list<SOCKET>::iterator iter;

	char c_message[1024] = {'\0',};
	char message_t[1032] = {'\0',};

	while (recv(c, c_message, sizeof(c_message), 0) > 0) {
		for (iter = clients.begin(); iter != clients.end(); ++iter) {
			if (*iter != c) {
//				send(*iter, c_message, sizeof(c_message), 0);
				sprintf_s(message_t, "%d >> %s", c, c_message);
				send(*iter, message_t, sizeof(message_t), 0);
			}
		}
	}

	cout << "(" << c << ") 종료" << endl;
	clients.remove(c);
	closesocket(c);
	return 0;
}

int main(int argc, char* argv[]) {

	if (argc != 2) {
		cout << "ex) " << argv[0] << "<Port>" << endl;
		return -1;
	}

	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {										// socket 초기화
		cout << "Socket reset error" << endl;
	}

	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);									// sockek 생성
	if (s == SOCKET_ERROR) {
		cout << "Socket create error" << endl;
		WSACleanup();
		return -1;
	}

	sockaddr_in server_addr, client_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[1]));
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {	// bind
		cout << "Bind error" << endl;
	}

	if (listen(s, 5) == SOCKET_ERROR) {														// listen
		cout << "Listen error" << endl;
	}
	
	while (1) {
		int c_addr_size = sizeof(client_addr);
		SOCKET c = accept(s, (struct sockaddr *)&client_addr, &c_addr_size);
		if (c == SOCKET_ERROR) {																// accept
			cout << "Accept error" << endl;
		}

		clients.push_back(c);
		char ip[16];
		inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
		cout << ip << " (" << c << ") connect!" << endl;
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, client_chat, (void*)&c, 0, NULL);
	}

	closesocket(s);
	WSACleanup();
	return 0;
}