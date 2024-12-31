#include "ClientManager.h"
#include <iostream>

void ClientManager::addClient(SOCKET clientSocket)
{
    auto clientInfo = std::make_unique<ClientInfo>();
    clientInfo->clientSocket = clientSocket;
    clientInfo->state = ClientState::WAITING_FOR_USERNAME;
    clients[clientSocket] = std::move(clientInfo);

    std::string usernameMessage = "Enter your username: ";
    uint32_t messageLength = htonl(usernameMessage.length());
    send(clientSocket, reinterpret_cast<const char *>(&messageLength), sizeof(messageLength), 0);
    int bytesSent = send(clientSocket, usernameMessage.c_str(), usernameMessage.length(), 0);
    if (bytesSent == SOCKET_ERROR)
    {
        std::cerr << "Failed to send username prompt, Err #" << WSAGetLastError() << std::endl;
    }
    else
    {
        std::cout << "Sent username prompt to client" << std::endl;
    }
}

void ClientManager::removeClient(SOCKET clientSocket)
{
    closesocket(clientSocket);
    clients.erase(clientSocket);
}

void ClientManager::processClientMessage(SOCKET clientSocket, const std::string &message)
{
    auto it = clients.find(clientSocket);
    if (it == clients.end())
    {
        std::cerr << "Unknown client socket" << std::endl;
        return;
    }

    ClientInfo *cinfo = it->second.get();
    cinfo->buffer.append(message);

    while (cinfo->buffer.size() >= sizeof(uint32_t))
    {
        uint32_t msgLength = ntohl(*reinterpret_cast<const uint32_t*>(cinfo->buffer.data()));
        if (cinfo->buffer.size() < sizeof(uint32_t) + msgLength)
        {
            break;
        }

        std::string line = cinfo->buffer.substr(sizeof(uint32_t), msgLength);
        cinfo->buffer.erase(0, sizeof(uint32_t) + msgLength);

        if (cinfo->state == ClientState::WAITING_FOR_USERNAME)
        {
            cinfo->username = line;
            cinfo->state = ClientState::CHATTING;
            std::string welcomeMessage = "Welcome, " + cinfo->username + "\n";
            uint32_t welcomeLength = htonl(welcomeMessage.length());
            send(cinfo->clientSocket, reinterpret_cast<const char*>(&welcomeLength), sizeof(welcomeLength), 0);
            int bytesSent = send(cinfo->clientSocket, welcomeMessage.c_str(), welcomeMessage.length(), 0);
            if (bytesSent == SOCKET_ERROR) {
                std::cerr << "Failed to send welcome message, Err #" << WSAGetLastError() << std::endl;
            } else {
                std::cout << "Sent welcome message to client" << std::endl;
            }
        }
        else
        {
            std::string fullmsg = cinfo->username + ": " + line + "\n";
            uint32_t fullmsgLength = htonl(fullmsg.length());
            for (const auto &pair : clients)
            {
                SOCKET receiver = pair.first;
                if (receiver != clientSocket)
                {
                    send(receiver, reinterpret_cast<const char *>(&fullmsgLength), sizeof(fullmsgLength), 0);
                    send(receiver, fullmsg.c_str(), fullmsg.length(), 0);
                }
            }
        }
    }
}

std::string ClientManager::getUsername(SOCKET clientSocket) {
    auto it = clients.find(clientSocket);
    if (it == clients.end())
    {
        std::cerr << "Unknown client socket" << std::endl;
        return "";
    }

    ClientInfo *cinfo = it->second.get();
    // std::cout << "getusername call, username: " << cinfo->username << std::endl;
    return cinfo->username;
}