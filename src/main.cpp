#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <map>
#include "network/NetworkUtils.h"
#include "client/ClientManager.h"

int main()
{
    if (NetworkUtils::initializeWinsock() != 0)
        return 1;

    SOCKET serverSocket = NetworkUtils::createServerSocket();
    if (serverSocket == INVALID_SOCKET)
        return 1;

    if (NetworkUtils::bindServerSocket(serverSocket) != 0)
        return 1;
    if (NetworkUtils::startListening(serverSocket) != 0)
        return 1;

    ClientManager clientManager;
    fd_set master;
    FD_ZERO(&master);
    FD_SET(serverSocket, &master);

    while (true)
    {
        fd_set copy = master;
        int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

        for (int i = 0; i < socketCount; i++)
        {
            SOCKET socket = copy.fd_array[i];
            if (socket == serverSocket)
            {
                sockaddr_in client;
                int clientSize = sizeof(client);
                SOCKET clientSocket = accept(serverSocket, (sockaddr *)&client, &clientSize);

                char host[NI_MAXHOST];
                char service[NI_MAXSERV];
                ZeroMemory(host, NI_MAXHOST);
                ZeroMemory(service, NI_MAXSERV);

                if (getnameinfo((sockaddr *)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
                {
                    std::cout << "New connection from " << host << " on port " << service << std::endl;
                }
                else
                {
                    inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
                    std::cout << "New connection from " << host << " on port " << ntohs(client.sin_port) << std::endl;
                }

                clientManager.addClient(clientSocket);
                FD_SET(clientSocket, &master);
            }
            else
            {
                char buff[16384];
                ZeroMemory(buff, 16384);
                int bytesReceived = recv(socket, buff, 16384, 0);
                if (bytesReceived <= 0)
                {
                    std::cout << "Client disconnected" << std::endl;
                    clientManager.removeClient(socket);
                    FD_CLR(socket, &master);
                }
                else
                {
                    std::string message(buff, bytesReceived);
                    std::string username = clientManager.getUsername(socket);
                    std::cout << username << ": " << message << std::endl;
                    clientManager.processClientMessage(socket, message);
                }
            }
        }
    }

    WSACleanup();
    return 0;
}