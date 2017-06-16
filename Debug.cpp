#include "Debug.hpp"

#include <cstdio>
#include <cstdarg>
#include <algorithm>
#include <string>
#ifdef _WIN32
#include <windows.h>
#endif


static char buffer[1024];


struct MatchPathSeparator
{
    bool operator()(char ch) const {
        return ch == '\\' || ch == '/';
    }
};

static std::string basename(std::string const& pathname)
{
    return std::string(std::find_if(pathname.rbegin(), pathname.rend(),
        MatchPathSeparator()).base(), pathname.end());
}

static std::string FormatString(const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
#if _WIN32
    vsprintf_s(buffer, format, arg);
#else
    vsprintf(buffer, format, arg);
#endif
    va_end(arg);
    return buffer;
}

void _Log(const std::string& filepath, int lineNum, const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
#if _WIN32
    vsprintf_s(buffer, format, arg);
#else
    vsprintf(buffer, format, arg);
#endif
    va_end(arg);

    std::string str = FormatString("%s\t(%s:%d)\n", buffer, basename(filepath).c_str(), lineNum);
#ifdef _WIN32
    OutputDebugStringA(str.c_str());
#endif
    printf("%s", str.c_str());
}
