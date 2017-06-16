#include <exception>
#include <cstdio>

#include "TCPClient.hpp"
#include "Debug.hpp"

int main()
{
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 0), &wsaData);
#endif

    try {
        TCPClient *client = new TCPClient("127.0.0.1", 12345);
        
        for (int i = 0; i < 10; i++) {
            client->Send("Hello!!\r\n");
            std::string reply = client->ReceiveString();
            DebugLog("[Reply] \"%s\"", reply.c_str());
        }
        
        delete client;
    } catch (std::exception& e) {
        DebugLog("Error: %s", e.what());
#ifdef _WIN32
        WSACleanup();
#endif
        return -1;
    }

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}
