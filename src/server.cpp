#include <WinSock2.h>
#include <iostream>
#include <string>
#include <thread>
#include "server.h"

void Server::handleClient(SOCKET clientSocket)
{
    char buff[30720] = {0};
    int bytes = recv(new_wsocket, buff, BUFFER_SIZE, 0);
    if (bytes < 0)
    {
        std::cout << "\nCouldn't read client request";
    }

    std::string serverMessage = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
    std::string response = "<html><h2>You are </h2><h1>NOT</h1><h2> a sigma!</h2></html>";
    serverMessage.append(std::to_string(response.size()));
    serverMessage.append("\n\n");
    serverMessage.append(response);

    int bytesSent = 0;
    int totalBytesSent = 0;
    while (totalBytesSent < serverMessage.size())
    {
        bytesSent = send(new_wsocket, serverMessage.c_str(), serverMessage.size(), 0);
        if (bytesSent < 0)
        {
            std::cout << "\nCould not send response";
        }
        totalBytesSent += bytesSent;
    }
    std::cout << "\nSent response to client";
    closesocket(clientSocket);
}

Server::Server(std::string ip, int port)
{
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        throw std::runtime_error("Couldn't initialize use of the Winsock DLL");
    }

    this->ip = ip;
    this->port = port;
    wsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (wsocket == INVALID_SOCKET)
    {
        WSACleanup();
        throw std::runtime_error("Couldn't create socket");
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip.c_str());
    server.sin_port = htons(port);
    server_len = sizeof(server);

    if (bind(wsocket, (SOCKADDR *)&server, server_len) != 0)
    {
        closesocket(wsocket);
        WSACleanup();
        throw std::runtime_error("Couldn't bind socket to server");
    }
}

Server::~Server()
{
    closesocket(wsocket);
    WSACleanup();
}

int Server::beginListening()
{
    if (listen(wsocket, 20) != 0)
    {
        std::cout << "Couldn't start listening";
        return -1;
    }
    std::cout << "Listening on " << ip << ":" << port << std::endl;

    while (true)
    {
        new_wsocket = accept(wsocket, (SOCKADDR *)&server, &server_len);
        if (new_wsocket == INVALID_SOCKET)
        {
            std::cout << "Couldn't accept\n";
            continue;
        }

        std::thread clientThread([this, clientSocket = new_wsocket]()
                                 { handleClient(new_wsocket); });
        clientThread.detach();
    }

    closesocket(wsocket);
    WSACleanup();
}