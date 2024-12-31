#pragma once

#include <map>
#include <memory>
#include <string>
#include <winsock2.h>

#include "ClientInfo.h"

class ClientManager {
public:
    void addClient(SOCKET clientSocket);
    void removeClient(SOCKET clientSocket);
    void processClientMessage(SOCKET clientSocket, const std::string &message);
    std::string getUsername(SOCKET clientSocket);

private:
    std::map<SOCKET, std::unique_ptr<ClientInfo>> clients;
};