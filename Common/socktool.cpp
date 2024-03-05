#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib,  "ws2_32.lib")
#ifndef SOCK_DEBUG
#define SOCK_DEBUG(...) (0)
#endif

SOCKET CreateSocket(
	CONST CHAR* pHost,
	CONST CHAR* pPort,
	LONG usec)
{
	SOCKET sock = INVALID_SOCKET;
	struct addrinfo* result = NULL,
		* ptr = NULL,
		hints;
	INT iResult;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	// Resolve the server address and port
	iResult = getaddrinfo(
		pHost,
		pPort,
		&hints,
		&result);
	if (iResult != 0) {
		SOCK_DEBUG("client: getaddrinfo error: %d\n",
			iResult);
		return INVALID_SOCKET;
	}
	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		sock = socket(
			ptr->ai_family,
			ptr->ai_socktype,
			ptr->ai_protocol);
		if (sock == INVALID_SOCKET) {
			SOCK_DEBUG("client: socket error: %ld\n",
				WSAGetLastError());
			return INVALID_SOCKET;
		}
		ULONG iMode = 1;
		iResult = ioctlsocket(
			sock,
			FIONBIO,
			&iMode);
		if (iResult != NO_ERROR)
		{
			SOCK_DEBUG("ioctlsocket error: %ld\n",
				iResult);
		}
		iResult = connect(
			sock,
			ptr->ai_addr,
			(INT)ptr->ai_addrlen);
		struct timeval tv;
		fd_set readfds, writefds;
		tv.tv_sec = usec / 1000000;
		tv.tv_usec = usec % 1000000;
		FD_ZERO(&readfds);
		FD_SET(sock, &readfds);
		FD_ZERO(&writefds);
		FD_SET(sock, &writefds);
		if ((iResult = select(
			0,
			&readfds,
			&writefds,
			NULL,
			&tv)) == SOCKET_ERROR ||
			(!FD_ISSET(sock, &readfds) &&
				!FD_ISSET(sock, &writefds))) {
			SOCK_DEBUG("select: fails: %d GetLastError: %d\n",
				iResult, GetLastError());
			closesocket(sock);
			sock = INVALID_SOCKET;
			continue;
		}
		iMode = 0;
		iResult = ioctlsocket(
			sock,
			FIONBIO,
			&iMode);
		if (iResult != NO_ERROR)
		{
			SOCK_DEBUG("ioctlsocket error: %ld\n",
				iResult);
		}
		break;
	}
	freeaddrinfo(result);
	return sock;
}

BOOL GetLocalIP(CHAR* buf, INT buflen)
{
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo* result = NULL,
		* ptr = NULL,
		hints, * res;
	BOOL iResult;
	INT status;
	CHAR hostname[128];
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	gethostname(hostname, sizeof(hostname));
	if ((status = getaddrinfo(
		hostname,
		NULL,
		&hints,
		&res)) != 0) {
		SOCK_DEBUG("getaddrinfo: %s\n",
			gai_strerror(status));
		return FALSE;
	}
	iResult = FALSE;
	buf[0] = 0;
	for (struct addrinfo* p = res; p != NULL; p = p->ai_next) {
		VOID* addr;
		CHAR ipver[100];
		if (p->ai_family == AF_INET) { // IPv4
			struct sockaddr_in* ipv4 =
				(struct sockaddr_in*)p->ai_addr;
			addr = &(ipv4->sin_addr);
			strcpy_s(ipver, 100, "IPv4");
			inet_ntop(p->ai_family, addr, buf, buflen);
			iResult = TRUE;
		}
		else { // IPv6
			struct sockaddr_in6* ipv6 =
				(struct sockaddr_in6*)p->ai_addr;
			addr = &(ipv6->sin6_addr);
			strcpy_s(ipver, 100, "IPv6");
		}
	}
	freeaddrinfo(res);
	return iResult;
}

INT RecvAll(
	SOCKET s,
	CHAR* buf,
	INT len,
	INT flags)
{
	INT iSize, iResult;
	if (len <= 0) {
		SOCK_DEBUG("RecvAll: invalid size: %d\n",
			len);
		return 0;
	}
	for (iSize = 0, iResult = 0;
		iSize < len;
		iSize += iResult) {
		iResult = recv(
			s,
			buf + iSize,
			len - iSize,
			flags);
		if (iResult == 0) {
			break;
		}
		if (iResult < 0) {
			SOCK_DEBUG("RecvAll: error %d\n",
				WSAGetLastError());
			return 0;
		}
	}
	return iSize;
}

INT SendAll(
	SOCKET s,
	CHAR* buf,
	INT len,
	INT flags)
{
	INT iSize, iResult;
	if (len <= 0) {
		SOCK_DEBUG("SendAll: invalid size: %d\n",
			len);
		return FALSE;
	}
	for (iSize = 0, iResult = 0;
		iSize < len;
		iSize += iResult) {
		iResult = send(
			s,
			buf + iSize,
			len - iSize,
			flags);
		if (iResult == 0) {
			break;
		}
		if (iResult < 0) {
			SOCK_DEBUG("SendAll: error %d\n",
				WSAGetLastError());
			return FALSE;
		}
	}
	return iSize;
}

// A sample program to scan server
BOOL ScanIPSample()
{
	CHAR ip[32];
	INT i1, i2, i3, i4;
	WSADATA wsaData;
	BOOL iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		SOCK_DEBUG("client: WSAStartup failed with error: %d\n",
			iResult);
		return FALSE;
	}
	GetLocalIP(ip, sizeof(ip));
	sscanf_s(ip, "%d.%d.%d.%d", &i1, &i2, &i3, &i4);
	for (INT i = 1; i < 255; i++) {
		if (i == i4) {
			continue;
		}
		sprintf_s(ip, sizeof(ip),
			"%d.%d.%d.%d", i1, i2, i3, i);
		SOCKET s = CreateSocket(ip, "9005", 100);
		if (s == INVALID_SOCKET) {
			printf("%s...Failed\n", ip);
		}
		else {
			printf("%s...Success\n", ip);
			send(s, "Success", 7, 0);
			shutdown(s, SD_BOTH);
			closesocket(s);
		}
	}
	WSACleanup();
	return TRUE;
}