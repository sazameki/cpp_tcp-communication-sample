//
//  TCPServer.hpp
//
//  Created by numata on 2017/06/13.
//  Copyright © 2017 Satoshi Numata. All rights reserved.
//

#ifndef TCPServer_hpp
#define TCPServer_hpp

#include "TCPClient.hpp"

class TCPServer
{
    bool    isClosed;
    int     port;
    int     serverSocket;

public:
    TCPServer(int port);
    ~TCPServer();
    
public:
    void        Close();
    TCPClient   *Accept();
};

#endif /* TCPServer_hpp */
