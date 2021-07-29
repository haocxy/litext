#include "thread.h"

#include <sstream>

#ifdef WIN32
#include <Windows.h>
#endif


namespace ThreadUtil
{

#ifndef NDEBUG

void setNameForCurrentThread(const std::string &name)
{
    if (name.empty()) {
        return;
    }
#ifdef WIN32
    constexpr UINT page = CP_UTF8;
    constexpr DWORD flags = MB_PRECOMPOSED;
    const char *mbstr = name.c_str();
    const int mblen = static_cast<int>(name.size());
    const int wstrlen = MultiByteToWideChar(page, flags, mbstr, mblen, nullptr, 0);
    if (wstrlen <= 0) {
        std::ostringstream ss;
        ss << "MultiByteToWideChar() failed because [" << GetLastError() << "]";
        throw std::logic_error(ss.str());
    }
    std::wstring wstr;
    wstr.resize(wstrlen);
    const int n = MultiByteToWideChar(page, flags, mbstr, mblen, wstr.data(), static_cast<int>(wstr.size()));
    if (n <= 0) {
        std::ostringstream ss;
        ss << "MultiByteToWideChar() failed because [" << GetLastError() << "]";
        throw std::logic_error(ss.str());
    }

    SetThreadDescription(GetCurrentThread(), wstr.c_str());
#endif
}

#else
void setNameForCurrentThread(const std::string &name) {}
#endif

}
