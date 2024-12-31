#pragma once

#include <winsock2.h>

class NetworkUtils {
public:
    static int initializeWinsock();
    static SOCKET createServerSocket();
    static int bindServerSocket(SOCKET serverSocket);
    static int startListening(SOCKET serverSocket);
};