#include <exception>
#include <cstdio>

#include "TCPClient.hpp"

int main()
{
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 0), &wsaData);
#endif

    try {
        TCPClient *client = new TCPClient("10.211.55.2", 12345);
        
        for (int i = 0; i < 10; i++) {
            client->Send("Hello!!\r\n");
        }
        
        delete client;
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
    }

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}
