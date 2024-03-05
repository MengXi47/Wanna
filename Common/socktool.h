#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#ifndef DEFAULT_BUFLEN
#define DEFAULT_BUFLEN 4096
#endif

#ifndef DEFAULT_CONNECT_TIMEOUT
#define DEFAULT_CONNECT_TIMEOUT 100000
#endif

SOCKET CreateSocket(CONST CHAR*, CONST CHAR*, LONG);
BOOL GetLocalIP(CHAR*, INT);
INT RecvAll(SOCKET, CHAR*, INT, INT);
INT SendAll(SOCKET, CHAR*, INT, INT);