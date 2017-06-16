//
//  TCPClient.cpp
//
//  Created by numata on 2017/06/13.
//  Copyright © 2017 Satoshi Numata. All rights reserved.
//

#include "TCPClient.hpp"

#include <unistd.h>
#include <stdexcept>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>


static std::string FormatString(const char* format, ...)
{
    static char buffer[1024];
    va_list marker;
    va_start(marker,format);
    vsprintf(buffer, format, marker);
    va_end(marker);
    return buffer;
}


/*!
     コンストラクタ
     指定されたホスト名とポート番号のソケットに接続を試みます。接続に失敗した場合にはruntime_error例外が発生します。
     useDelayにfalseが指定された場合、Nagleアルゴリズムによって小さな送信パケットをまとめることを無効化します。
     @param hostname_   接続先のソケットのホスト名
     @param port_       接続先のソケットのポート番号
     @param useDelay    データ送信にNagleアルゴリズムによるパケット数軽減を使用するかを指定します
 */
TCPClient::TCPClient(const std::string& hostname_, int port_, bool useDelay)
{
    hostname = hostname_;
    port = port_;
    isClosed = true;
    
    // ソケットの作成
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        throw std::runtime_error("Failed to create a socket.");
    }
    isClosed = false;
    
    // ディレイなしの送信の設定
    if (!useDelay) {
        int noDelay = 1;
        if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &noDelay, sizeof(noDelay)) == -1) {
            Close();
            throw std::runtime_error("Failed to set a socket option (TCP_NODELAY).");
        }
    }
    
    // ホスト名の解決
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port_);
    in_addr addr;
    addr.s_addr = inet_addr(hostname_.c_str());
    if (addr.s_addr == -1) {
        hostent *host = gethostbyname(hostname.c_str());
        if (!host) {
            throw std::runtime_error("Failed to get a host entry.");
        }
        memcpy(&addr, (in_addr *)*host->h_addr_list, sizeof(in_addr));
    }
    hostname = std::string(inet_ntoa(addr));
    serverAddr.sin_addr = addr;
    
    // ホストへの接続
    if (connect(sock, (sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        Close();
        throw std::runtime_error(FormatString("Failed to connect a TCP socket to server: %s:%d", hostname.c_str(), port));
    }
}

/*!
     コンストラクタ
     @param sock_       ソケットディスクリプタ
     @param clientAddr  接続先のソケットのアドレス情報を格納した構造体
 */
TCPClient::TCPClient(int sock_, sockaddr_in& clientAddr)
{
    sock = sock_;
    isClosed = false;
    
    hostname = std::string(inet_ntoa(clientAddr.sin_addr));
    port = ntohs(clientAddr.sin_port);
}

/*!
     デストラクタ
 */
TCPClient::~TCPClient()
{
    Close();
}

/*!
     接続先のホスト名（IPアドレス）を取得します。
     @return    接続先のホスト名
 */
std::string TCPClient::GetRemoteHost() const
{
    return hostname;
}

/*!
     接続先のポート番号を取得します。
     @return    接続先のポート番号
 */
int TCPClient::GetRemotePort() const
{
    return port;
}

/*!
     ソケットをクローズし、接続を切断します。
 */
void TCPClient::Close()
{
    if (isClosed) {
        return;
    }
    close(sock);
    sock = -1;
}

/*!
     ソケットが通信先に接続されているかどうかをリターンします。
     ソケットの通信状態はReceive()メソッドまたはSend()メソッドの実行時にチェックされるため、このメソッドがtrueを返しても。
     @return    ソケットが通信先に接続されていればtrue、そうでなければfalse
 */
bool TCPClient::IsConnected() const
{
    return !isClosed;
}

/*!
     与えられたバッファを使用してデータを受信します。
     このメソッドは、ReceiveString()メソッドで使用するバッファとは独立してソケットからデータを受信します。ReceiveString()メソッドで受信したデータがバッファ内に残っていた場合に、そのデータよりも後に送信されたデータがこのReceive()メソッドによって先に取り出される場合があります。
     受信時にソケットが切断されていた場合、またはエラーが発生した場合には、Close()メソッドが自動的に呼ばれます。
     @return 受信したデータのバイト数。ソケットが正しく切断されていた場合は0、エラーの場合は-1。
 */
ssize_t TCPClient::Receive(char *inBuffer, size_t bufferSize)
{
    ssize_t len = recv(sock, inBuffer, bufferSize, 0);
    if (len <= 0) {
        Close();
    }
    return len;
}

/*!
     1行分の文字列データを受信します。
     複数行のデータが同時に受信される場合もありますが、改行コードで分割された上でクライアント内のキューに格納され、次にこのメソッドが呼ばれた時には格納分の文字列が先頭から順にリターンされます。
     受信時にソケットが切断されていた場合、またはエラーが発生した場合には、Close()メソッドが自動的に呼ばれます。
     @return    受信した文字列
 */
std::string TCPClient::ReceiveString()
{
    // バッファ中の文字列がある場合はそれをリターンする
    if (bufferedMessages.size() > 0) {
        std::string ret = bufferedMessages.front();
        bufferedMessages.pop();
        return ret;
    }
    
    // 新しい文字列を受信する
    ssize_t len = Receive(buffer, TCP_CLIENT_BUFFER_SIZE);
    if (len <= 0) {
        return "";
    }
    
    // 受信した文字列を改行コードで分割してバッファに追加する
    std::string str(buffer, len);
    std::string delim("\r\n");
    std::string::size_type pos = 0;
    while (pos != std::string::npos) {
        std::string::size_type p = str.find(delim, pos);
        if (p == std::string::npos) {
            std::string last = str.substr(pos);
            if (last.length() > 0) {
                bufferedMessages.push(last);
            }
            break;
        } else {
            bufferedMessages.push(str.substr(pos, p - pos));
        }
        pos = p + delim.size();
    }
    if (bufferedMessages.size() == 0) {
        throw std::runtime_error("Failed to split received message.");
    }
    
    // 分割した文字列バッファの先頭にある文字列をリターンする
    std::string ret = bufferedMessages.front();
    bufferedMessages.pop();
    return ret;
}

/*!
     データを送信します。
     ソケットがクローズされた後にこのメソッドが呼ばれた場合には、runtime_error例外が発生します。
     送信時にソケットが切断されていた場合、またはエラーが発生した場合には、Close()メソッドが自動的に呼ばれます。
     @return    送信されたデータのバイト数
 */
ssize_t TCPClient::Send(char *buffer, size_t length)
{
    if (isClosed) {
        throw std::runtime_error("TCPClient::Send() was called after the socket is closed.");
    }
    ssize_t len = send(sock, buffer, length, 0);
    if (len <= 0) {
        Close();
    }
    return len;
}

/*!
     文字列データを送信します。
     ソケットがクローズされた後にこのメソッドが呼ばれた場合には、runtime_error例外が発生します。
     送信時にソケットが切断されていた場合、またはエラーが発生した場合には、Close()メソッドが自動的に呼ばれます。
     @return    送信されたデータのバイト数
 */
ssize_t TCPClient::Send(const std::string& str)
{
    return Send((char *)str.c_str(), (size_t)str.length());
}


