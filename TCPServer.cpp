//
//  TCPServer.cpp
//
//  Created by numata on 2017/06/13.
//  Copyright © 2017 Satoshi Numata. All rights reserved.
//

#include "TCPServer.hpp"

#include <stdexcept>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>


/*!
     コンストラクタ
     ポート番号を指定してTCPサーバを起動します。
     サーバ起動に失敗した場合、runtime_error例外が発生します。
 */
TCPServer::TCPServer(int port_)
{
    isClosed = true;
    port = port_;

    // ソケットの作成
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        throw std::runtime_error("Failed to create a server socket.");
    }
    isClosed = false;

    // ポート再利用の設定
    int reuseFlag = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuseFlag,sizeof(reuseFlag)) == -1) {
        Close();
        throw std::runtime_error("Failed to set the reuse option to a server socket.");
    }

    // ソケットの設定
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    if (bind(serverSocket, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        Close();
        throw std::runtime_error("Failed to bind a server socket.");
    }
    
    // ソケットを接続可能状態にする
    if (listen(serverSocket, SOMAXCONN) == -1) {
        Close();
        throw std::runtime_error("Failed to listen a server socket.");
    }
}

/*!
     デストラクタ
 */
TCPServer::~TCPServer()
{
    Close();
}

/*!
     ソケットをクローズし、接続を切断します。
 */
void TCPServer::Close()
{
    if (isClosed) {
        return;
    }
    close(serverSocket);
    serverSocket = -1;
    isClosed = true;
}

/*!
     クライアントを受け付けます。
     エラーが発生した場合には、runtime_error例外がスローされます。
 */
TCPClient *TCPServer::Accept()
{
    sockaddr_in clientAddr;
    unsigned int addrLen = sizeof(clientAddr);
    int sock = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrLen);
    if (sock == -1) {
        Close();
        throw std::runtime_error("Failed to accept a TCP client.");
    }
    return new TCPClient(sock, clientAddr);
}


