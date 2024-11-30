#ifndef SERVER_H_
#define SERVER_H_

#include <WinSock2.h>
#include <string>

class Server
{
private:
    SOCKET wsocket;
    SOCKET new_wsocket;
    struct sockaddr_in server;
    int server_len;
    int BUFFER_SIZE = 30720;
    std::string ip;
    int port;
    WSADATA wsaData;

    void handleClient(SOCKET clientSocket);

public:
    Server(std::string ip, int port);
    ~Server();
    int beginListening();
};

#endif