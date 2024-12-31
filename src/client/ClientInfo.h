#pragma once

#include <string>
#include <winsock2.h>

enum class ClientState {
    WAITING_FOR_USERNAME,
    CHATTING
};

struct ClientInfo {
    SOCKET clientSocket;
    std::string username;
    ClientState state;
    std::string buffer;
};