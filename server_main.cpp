#include <cstdio>
#include <iostream>
#include <pthread.h>

#include "TCPServer.hpp"


static void *ThreadProc(void *pData);


int main()
{
    try {
        int port = 12345;
        TCPServer *server = new TCPServer(port);
        
        std::cout << "TCP server is running on port " << port << std::endl;
        
        bool isRunning = true;
        while (isRunning) {
            TCPClient *client = server->Accept();
            pthread_t tid;
            pthread_create(&tid, NULL, ThreadProc, client);
        }
        
        delete server;
    } catch (std::exception& e) {
        printf("Error: %s\n", e.what());
    }
    
    return 0;
}

void *ThreadProc(void *pData)
{
    TCPClient *client  = (TCPClient *)pData;
    printf("[%s:%d] Client was accepted.\n", client->GetRemoteHost().c_str(), client->GetRemotePort());
    
    while (true) {
        std::string str = client->ReceiveString();
        if (str.length() == 0) {
            break;
        }
        printf("[%s:%d] \"%s\"\n", client->GetRemoteHost().c_str(), client->GetRemotePort(), str.c_str());
    }
    printf("[%s:%d] Client was disconnected.\n", client->GetRemoteHost().c_str(), client->GetRemotePort());
    
    delete client;
    return NULL;
}

