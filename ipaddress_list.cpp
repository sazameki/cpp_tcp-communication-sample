//
//  ipaddress_list.cpp
//
//  Created by numata on 2017/06/17.
//  Copyright © 2017 Satoshi Numata. All rights reserved.
//

#include "ipaddress_list.hpp"

#include <cstdio>
#include <cstdlib>
#include "Debug.hpp"


#ifdef _WIN32

static char buffer[1024];

static std::string FormatString(const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    vsprintf_s(buffer, format, arg);
    va_end(arg);
    return buffer;
}

std::map<std::string, std::string> GetLocalIpAddresses()
{
    std::map<std::string, std::string> ret;

    SOCKET sock = WSASocket(AF_INET, SOCK_DGRAM, 0, 0, 0, 0);
    if (sock == SOCKET_ERROR) {
        return ret;
    }
    INTERFACE_INFO if_list[20];
    unsigned long ret_bytes;
    if (WSAIoctl(sock, SIO_GET_INTERFACE_LIST, NULL, 0, &if_list,
            sizeof(if_list), &ret_bytes, NULL, NULL) == SOCKET_ERROR)
    {
        closesocket(sock);
        return ret;
    }
    closesocket(sock);

    int interfaceCount = ret_bytes / sizeof(INTERFACE_INFO);

    for (int i = 0; i < interfaceCount; i++) {
        if (!(if_list[i].iiFlags & IFF_UP) || (if_list[i].iiFlags & IFF_LOOPBACK) && i != interfaceCount - 1) {
            continue;
        }
        sockaddr_in *sa = (sockaddr_in*)&(if_list[i].iiAddress);
        if (sa->sin_addr.s_addr != 0 && sa->sin_family == AF_INET) {
            if (sa->sin_addr.s_net == 0 || sa->sin_addr.S_un.S_un_b.s_b1 == 169 && sa->sin_addr.S_un.S_un_b.s_b2 == 254) {
                continue;
            }
            std::string ifname = FormatString("if-%d", i);
            ret[ifname] = inet_ntoa(sa->sin_addr);
        }
    }

    return ret;
}

#else

std::map<std::string, std::string> GetLocalIpAddresses()
{
    std::map<std::string, std::string> ret;

    ifaddrs *ifaddr;
    if (getifaddrs(&ifaddr) == -1) {
        return ret;
    }
    
    ifaddrs *ifa = ifaddr;
    char hostname[NI_MAXHOST];
    while (ifa) {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET) {
            int s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), hostname, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            if (s == 0) {
                ret[ifa->ifa_name] = hostname;
            }
        }
        ifa = ifa->ifa_next;
    }
    
    freeifaddrs(ifaddr);

    return ret;
}

#endif

