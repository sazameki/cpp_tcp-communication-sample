#include <cstdio>
#include <iostream>
#ifdef _WIN32
#define _WINSOCKAPI_
#include <windows.h>
#else
#include <pthread.h>
#endif

#include "TCPServer.hpp"
#include "Debug.hpp"


#ifdef _WIN32
static DWORD WINAPI ThreadProc(LPVOID pData);
#else
static void *ThreadProc(void *pData);
#endif


int main()
{
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 0), &wsaData);
#endif

    try {
        int port = 12345;
        TCPServer *server = new TCPServer(port);
        
        DebugLog("TCP server is running on port %d.", port);
        
        bool isRunning = true;
        while (isRunning) {
            TCPClient *client = server->Accept();
#ifdef _WIN32
            CreateThread(NULL, 0, ThreadProc, client, 0, NULL);
#else
            pthread_t tid;
            pthread_create(&tid, NULL, ThreadProc, client);
#endif
        }
        
        delete server;
    } catch (std::exception& e) {
        DebugLog("Error: %s", e.what());
    }
    
#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}

#ifdef _WIN32
static DWORD WINAPI ThreadProc(LPVOID pData)
#else
static void *ThreadProc(void *pData)
#endif
{
    TCPClient *client  = (TCPClient *)pData;
    DebugLog("[%s:%d] Client was accepted.", client->GetRemoteHost().c_str(), client->GetRemotePort());
    
    while (true) {
        std::string str = client->ReceiveString();
        if (str.length() == 0) {
            break;
        }
        DebugLog("[%s:%d] \"%s\"", client->GetRemoteHost().c_str(), client->GetRemotePort(), str.c_str());

        client->Send("OK\r\n");
    }
    DebugLog("[%s:%d] Client was disconnected.", client->GetRemoteHost().c_str(), client->GetRemotePort());
    
    delete client;
    return NULL;
}

