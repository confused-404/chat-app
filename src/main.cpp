#include <iostream>
#include <WS2tcpip.h>
#include <string>

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

                std::string port;

                if (getnameinfo((sockaddr *)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
                {
                    port = service;
                }
                else
                {
                    inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
                    port = ntohs(client.sin_port);
                }

                FD_SET(clientSocket, &master);
                
                std::string welcomeMessage = "Welcome, User " + std::to_string(master.fd_count-1) + "\n";

                send(clientSocket, welcomeMessage.c_str(), welcomeMessage.size()+1, 0);
            }
            else
            {
                char buff[4096];
                ZeroMemory(buff, 4096);

                int bytesReceived = recv(socket, buff, 4096, 0);
                if (bytesReceived <= 0) {
                    closesocket(socket);
                    FD_CLR(socket, &master);
                } else {
                    for (int i = 0; i < master.fd_count; i++) {
                        SOCKET receiver = master.fd_array[i];
                        if (receiver != serverSocket && receiver != socket) {
                            send(receiver, buff, bytesReceived, 0);
                        }
                    }
                }
            }
        }
    }

    WSACleanup();
    return 0;
}