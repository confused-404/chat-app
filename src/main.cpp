#include <iostream>
#include <WS2tcpip.h>

int main()
{
    WSAData wsData;
    WORD ver = MAKEWORD(2, 2);

    int wsOk = WSAStartup(ver, &wsData);
    if (wsOk != 0)
    {
        std::cerr << "Can't initialize WinSock" << std::endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Can't create server socket" << std::endl;
        return 1;
    }

    sockaddr_in server_hint;
    server_hint.sin_family = AF_INET;
    server_hint.sin_port = htons(54000);
    server_hint.sin_addr.S_un.S_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr *)&server_hint, sizeof(server_hint)) == SOCKET_ERROR)
    {
        std::cerr << "Bind failed, Err #" << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Listen failed, Err #" << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server listening on port 54000..." << std::endl;

    sockaddr_in client;
    int clientSize = sizeof(client);

    SOCKET clientSocket = accept(serverSocket, (sockaddr *)&client, &clientSize);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Accept failed, Err #" << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Client connected!" << std::endl;

    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    ZeroMemory(host, NI_MAXHOST);
    ZeroMemory(service, NI_MAXSERV);

    if (getnameinfo((sockaddr *)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
    {
        std::cout << host << " connected on port " << service << std::endl;
    }
    else
    {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        std::cout << host << " connected on port " << ntohs(client.sin_port) << std::endl;
    }

    closesocket(serverSocket);

    char buff[4096];

    while (true)
    {
        ZeroMemory(buff, 4096);

        int bytesReceived = recv(clientSocket, buff, 4096, 0);
        if (bytesReceived == SOCKET_ERROR)
        {
            std::cerr << "Error receiving data: " << WSAGetLastError() << std::endl;
            break;
        }

        if (bytesReceived == 0)
        {
            std::cout << "Client disconnected" << std::endl;
            break;
        }
        buff[bytesReceived] = '\0';

        for (int i = 0; i < bytesReceived; i++)
        {
            if (buff[i] == '\r')
            {
                buff[i] = '\n';
            }
        }

        std::cout << "Received: " << buff << std::endl;

        send(clientSocket, buff, bytesReceived, 0);
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}