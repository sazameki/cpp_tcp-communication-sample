#include <exception>
#include <cstdio>

#include "TCPClient.hpp"


int main()
{
    try {
        TCPClient *client = new TCPClient("127.0.0.1", 12345);
        
        for (int i = 0; i < 10; i++) {
            client->Send("Hello!!\r\n");
        }
        
        delete client;
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
    }
    
    return 0;
}
