//
//  TCPClient.hpp
//
//  Created by numata on 2017/06/13.
//  Copyright © 2017 Satoshi Numata. All rights reserved.
//

#ifndef TCPClient_hpp
#define TCPClient_hpp

#include <queue>
#include <string>
#include <netinet/in.h>

#define TCP_CLIENT_BUFFER_SIZE  1024

class TCPClient
{
    bool        isClosed;
    std::string hostname;
    int         port;
    int         sock;
    char        buffer[TCP_CLIENT_BUFFER_SIZE];
    std::queue<std::string>    bufferedMessages;
    
public:
    TCPClient(const std::string& hostname, int port, bool useDelay = true);
    TCPClient(int sock, sockaddr_in& clientAddr);
    ~TCPClient();
    
public:
    std::string GetRemoteHost() const;
    int         GetRemotePort() const;
    bool        IsConnected() const;
    void        Close();
    ssize_t     Receive(char *inBuffer, size_t bufferSize);
    std::string ReceiveString();
    ssize_t     Send(char *buffer, size_t length);
    ssize_t     Send(const std::string& str);
};

#endif /* TCPClient_hpp */


