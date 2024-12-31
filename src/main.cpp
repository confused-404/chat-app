#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <map>

enum class ClientState
{
    WAITING_FOR_USERNAME,
    CHATTING
};
struct ClientInfo
{
    SOCKET clientSocket;
    std::string username;
    ClientState state;
    std::string buffer;
};

int initializeWinsock()
{
    WSAData wsData;
    WORD ver = MAKEWORD(2, 2);

    int wsOk = WSAStartup(ver, &wsData);
    if (wsOk != 0)
    {
        std::cerr << "Can't initialize WinSock" << std::endl;
        return 1;
    }
    return 0;
}

SOCKET createServerSocket()
{
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Can't create server socket" << std::endl;
        return INVALID_SOCKET;
    }
    return serverSocket;
}

int bindServerSocket(SOCKET serverSocket)
{
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
    return 0;
}

int startListening(SOCKET serverSocket)
{
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Listen failed, Err #" << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    return 0;
}

int main()
{
    initializeWinsock();
    SOCKET serverSocket = createServerSocket();
    bindServerSocket(serverSocket);
    startListening(serverSocket);

    fd_set master;
    FD_ZERO(&master);
    FD_SET(serverSocket, &master);
    std::map<SOCKET, ClientInfo *> clients;

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

                std::string usernameMessage = "Enter your username: ";

                send(clientSocket, usernameMessage.c_str(), usernameMessage.length(), 0);

                ClientInfo *cinfo = new ClientInfo;
                cinfo->clientSocket = clientSocket;
                cinfo->username = "";
                cinfo->state = ClientState::WAITING_FOR_USERNAME;
                clients[clientSocket] = cinfo;
            }
            else
            {
                auto it = clients.find(socket);

                if (it == clients.end())
                {
                    std::cerr << "Foreign socket sending data" << std::endl;
                }

                ClientInfo *cinfo = it->second;

                char buff[8192];
                ZeroMemory(buff, 8192);

                int bytesReceived = recv(socket, buff, 8192, 0);
                if (bytesReceived <= 0)
                {
                    closesocket(socket);
                    FD_CLR(socket, &master);
                }
                else
                {
                    cinfo->buffer.append(buff, bytesReceived);

                    size_t newlinePos;
                    while ((newlinePos = cinfo->buffer.find('\n')) != std::string::npos)
                    {
                        std::string line = cinfo->buffer.substr(0, newlinePos);
                        cinfo->buffer.erase(0, newlinePos + 1);

                        if (cinfo->state == ClientState::WAITING_FOR_USERNAME)
                        {
                            cinfo->username = line;
                            cinfo->state = ClientState::CHATTING;
                            std::string welcomeMessage = "Welcome, " + cinfo->username + "\n";
                            send(cinfo->clientSocket, welcomeMessage.c_str(), welcomeMessage.length(), 0);

                            std::cout << "Sent to " << cinfo->username << ": " << welcomeMessage;
                        }
                        else
                        {
                            std::string fullmsg = cinfo->username + ": " + line + "\n";

                            std::cout << "Received from " << cinfo->username << ": " << line << std::endl;
                            std::cout << cinfo->username << std::endl;

                            for (int i = 0; i < master.fd_count; i++)
                            {
                                SOCKET receiver = master.fd_array[i];
                                if (receiver != serverSocket && receiver != socket)
                                {
                                    send(receiver, fullmsg.c_str(), fullmsg.length(), 0);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    WSACleanup();
    return 0;
}