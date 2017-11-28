// TIC TAC TOR

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include "Board.h"
#include "rsa_user.h"

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

//according to number of middle nodes you created
#define NUM_NODES 3

int keys[NUM_NODES] = { 0 };

int __cdecl main(int argc, char **argv)
{

	// make the generated rsa keys psuedo-random
	srand(std::time(NULL));
	std::cout << "Node 0 - client" << std::endl;


	bool isAlive = true;
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	char *sendbuf = "initialized";
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// Validate the parameters
	if (argc != 2) {
		printf("usage: %s server-name\n", argv[0]);
		return 1;
	}

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL;ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	// initialize rsa_user for rsa initialization
	rsa_user ru;

	// initialize board for running backend tic-tac-toe 
	Board b;
	
	// share key using rsa with every node
	for (int i = 0; i < NUM_NODES; i++)
	{

		// send required args to the rsa-peer using rsa_user.get_init_msg()
		std::string rsaMsg = ru.get_init_msg();
		int length = strlen(rsaMsg.c_str());
		
		// encrypt the key with existing nodes, so they will decrypt it on the way.
		for (int i : keys) {
			r_encrypt(rsaMsg, i);
		}
		
		rsaMsg[length] = '\0';
		std::cout << "Sending rsa init message: " << rsaMsg << std::endl;
		
		iResult = send(ConnectSocket, rsaMsg.c_str(), (int)strlen(rsaMsg.c_str()), 0);
		if (iResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}

		// recv rsa message back from the peer
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		recvbuf[iResult] = '\0';
		std::cout << "Rcvd rsa back msg: " << recvbuf << std::endl;
		
		// decrypt with every key created so far
		std::string originalKey = recvbuf;
		for (int a = 0; a < i; a++) {//we want to decrypt until this stage
			originalKey = r_decrypt(originalKey, keys[a]);
		}

		// update keys[] according to decrypted data got in originalKey
		keys[i] = ru.calculate_original_key(originalKey);
		std::cout << "Msg decrypted = " << keys[i] << std::endl;
		
		// reset parameters for the next rsa_sharing
		ru.reset_params();

		// inform log
		std::cout << "Connection with peer-" << i << " succeded, keys:";
		for (int k : keys) {
			std::cout << " [" << k << "] ";
		}
		std::cout << std::endl;
	}

	// end make rsa, start of tic-tac-toe information
	std::cout << b.instrucations << b.getStringBoard() << std::endl;

	// receive until the peer closes the connection / lose / win / tie situation
	do {
		std::string sendInfo;
		int update = -1;

		do {

			// sendInfo is the place the player will put his sign
			std::cin >> sendInfo;

			try {
				update = b.update(std::stoi(sendInfo), 1);
			}
			catch (std::invalid_argument e) {
				std::cout << "Please submit numbers from 1-9.";
				continue;
			}

			// inform the user about the board - getStringBoard() and his move - updateHandler(update)
			std::cout << b.getStringBoard() << std::endl;
			std::cout << b.updateHandler(update) << std::endl;

		} while (update < 0);

		// encrypt message - we want the first node to be the latest encryption
		for (int l = (NUM_NODES-1); l > -1; l--)
		{
			r_encrypt(sendInfo, keys[l]);
		}
		
		std::cout << "Sending encrypted message: " << sendInfo << std::endl;
		iResult = send(ConnectSocket, sendInfo.c_str(), (int)strlen(sendbuf), 0);
		if (iResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}

		// update > 0  means win, lose, or tie
		if (update > 0) { break; }

		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
	
		if (iResult > 0){
	
			//get the message without the rubbish afterwards
			recvbuf[iResult] = '\0';
			std::string partlyDecrypted = recvbuf;
			for (int a = 0; a < (NUM_NODES); a++) {//we want to decrypt until this stage
				partlyDecrypted = r_decrypt(partlyDecrypted, keys[a]);
			}
			
			int dycrptedMsg = std::stoi(partlyDecrypted); //dcrpt

			//update board
			int updateRcvd = b.update(dycrptedMsg, 2);
			if (updateRcvd > 0) { 
				std::cout << b.updateHandler(updateRcvd) << std::endl;
				isAlive = false;
				break; 
			}
			else {
				std::cout << b.getStringBoard() << std::endl;
			}
		}

		if (update > 0) { 
			std::cout << b.getStringBoard() << std::endl;
			isAlive = false;
		}

	} while (isAlive);

	// shutdown the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf(	"shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		system("pause");
		return 1;
	}

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();
	
	int i;
	std::cin >> i;
	return 0;
}