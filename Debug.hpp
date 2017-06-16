//
//  Debug.hpp
//
//  Created by numata on 2017/06/15.
//  Copyright © 2017 Satoshi Numata. All rights reserved.
//

#ifndef Debug_hpp
#define Debug_hpp

#include <string>

#define DebugLog(...)   { _Log(__FILE__, __LINE__, __VA_ARGS__); }
void _Log(const std::string& filepath, int lineNum, const char* format, ...);

#endif /* Debug_hpp */


