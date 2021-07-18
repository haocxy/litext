#include "single_thread_strand.h"

#include <sstream>

#ifdef WIN32
#include <Windows.h>
#endif

static void setNameForCurrentThread(const std::string &name)
{
    if (name.empty()) {
        return;
    }
#ifdef WIN32
    std::wstring widestr;
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


SingleThreadStrand::SingleThreadStrand(const std::string &name)
    : name_(name)
    , thread_([this] { loop(); })
{

}

SingleThreadStrand::~SingleThreadStrand()
{
    running_ = false;
    thread_.join();
}

void SingleThreadStrand::post(std::function<void()> &&f)
{
    queue_.push(std::move(f));
}

void SingleThreadStrand::loop()
{
    setNameForCurrentThread(name_);

    while (running_) {
        std::optional<std::function<void()>> f = queue_.pop();
        if (f) {
            (*f)();
        }
    }
}
