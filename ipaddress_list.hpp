//
//  ipaddress_list.hpp
//
//  Created by numata on 2017/06/17.
//  Copyright © 2017 Satoshi Numata. All rights reserved.
//

#ifndef ipaddress_list_hpp
#define ipaddress_list_hpp

#include <map>
#include <string>

#ifdef _WIN32
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iptypes.h>
#include <iphlpapi.h>
#pragma comment( lib, "ws2_32.lib" )
#else
#include <ifaddrs.h>    // getifaddrs()
#include <netdb.h>      // NI_MAXHOST
#endif

std::map<std::string, std::string> GetLocalIpAddresses();

#endif /* ipaddress_list_hpp */
