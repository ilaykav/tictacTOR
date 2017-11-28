#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <mutex>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "iostream"
#include <string>
#include "Board.h"
#include "rsa_user.h"
#include "rsa.cpp"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27017"

//server

int __cdecl EndNodeMain(void)
{
	srand(std::time(NULL));
	std::cout << "node1 - Server" << std::endl;
	bool isAlive = true;
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		int i;
		std::cin >> i;
		return 1;
	}

	Board b;
	rsa_user ru;

	// make rsa

	iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
	recvbuf[iResult] = '\0';
	std::cout << "Rcvd msg for rsa init: " << recvbuf << std::endl;
	int key = generate_key();
	std::cout << "Generated key = " << key << std::endl;
	std::string rsaBackMsg = calculate_reponse(recvbuf, key);

	iResult = send(ClientSocket, rsaBackMsg.c_str(), (int)strlen(rsaBackMsg.c_str()), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}
	//end make rsa

	std::cout << b.instrucations << b.getStringBoard() << std::endl;
	// Receive until the peer shuts down the connection
	do {
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);

		if (iResult > 0) {

			// get the message without the rubbish afterwards
			recvbuf[iResult] = '\0';
			int dycrptedMsg = std::stoi(r_decrypt(recvbuf, key)); //dcrpt

																  // update board
			int updateRcvd = b.update(dycrptedMsg, 1);
			if (updateRcvd > 0) {
				std::cout << b.updateHandler(updateRcvd) << std::endl;
				isAlive = false;
				break;
			}
			else {
				std::cout << b.getStringBoard() << std::endl;
			}

			// take arg from user and send it 
			std::string sendInfo;
			int update = -1;

			do {
				std::cin >> sendInfo;
				try {
					update = b.update(std::stoi(sendInfo), 2);
				}
				catch (std::invalid_argument e) {
					std::cout << "Please submit numbers from 1-9.";
					continue;
				}
				std::cout << b.getStringBoard() << std::endl;
				std::cout << b.updateHandler(update) << std::endl;
			} while (update < 0);

			r_encrypt(sendInfo, key);// encrypt

			iSendResult = send(ClientSocket, sendInfo.c_str(), iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				int i;
				std::cin >> i;
				return 1;
			}

			if (update > 0) { isAlive = false; }

		}
	} while (isAlive);

	// No longer need server socket
	closesocket(ListenSocket);

	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		int i;
		std::cin >> i;
		return 1;
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();

	int i;
	std::cin >> i;
	return 0;
}
