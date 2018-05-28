#include <winsock2.h>					// socket
#include <Ws2tcpip.h>					// inet_pton
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <process.h>

using namespace std;

unsigned WINAPI send(void* sock) {
	SOCKET s = *(SOCKET*)sock;
	while (1) {
		char send_msg[1024];

		cin.getline(send_msg, 1024);
		if (strcmp(send_msg, "exit") == 0) {
			closesocket(s);
			exit(0);
		}
		send(s, send_msg, sizeof(send_msg), 0);
	}
	return 0;
}

unsigned WINAPI recv(void* sock) {
	SOCKET s = *(SOCKET*)sock;
	char recv_msg[1032];
	while (1) {
		recv(s, recv_msg, sizeof(recv_msg), 0);
		cout << recv_msg << endl;
	}
	return 0;
}

int main(int argc, char* argv[]) {

	if (argc != 3) {
		cout << "ex) " << argv[0] << " <IP> <Port>" << endl;
		return -1;
	}

	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {										// socket 초기화
		cout << "Socket reset error" << endl;
	}

	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);									// sockek 생성
	if (s == INVALID_SOCKET) {
		cout << "Socket create error" << endl;
		WSACleanup();
		return -1;
	}

	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(argv[2]));
//	addr.sin_addr.s_addr = inet_addr(argv[1]);
	inet_pton(AF_INET, argv[1], &addr.sin_addr.s_addr);

	if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) {
		cout << "Connect error" << endl;
		closesocket(s);
		return -1;
	}

	cout << argv[1] << endl;
	
	HANDLE send_Thread = (HANDLE)_beginthreadex(NULL, 0, send, (void*)&s, 0, NULL);
	HANDLE recv_Thread = (HANDLE)_beginthreadex(NULL, 0, recv, (void*)&s, 0, NULL);

	WaitForSingleObject(send_Thread, INFINITE);
	WaitForSingleObject(recv_Thread, INFINITE);

	closesocket(s);
	WSACleanup();
	return 0;
}