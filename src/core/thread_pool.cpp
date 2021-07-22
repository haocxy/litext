#include "thread_pool.h"

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


ThreadPool::ThreadPool(const std::string &name, int threadCount)
    : name_(name)
{
    for (int i = 0; i < threadCount; ++i) {
        threads_.push_back(std::thread([this, i]() {
            std::ostringstream ss;
            ss << name_ << "(" << i << ")";
            setNameForCurrentThread(ss.str());
            loop();
        }));
    }
}

ThreadPool::~ThreadPool()
{
    stopping_ = true;
    queue_.wakeupProducer();
    queue_.wakeupConsumer();
    for (std::thread &t : threads_) {
        t.join();
    }
}

void ThreadPool::post(std::function<void()> &&f)
{
    queue_.push(std::move(f));
}

bool ThreadPool::isStopping() const
{
    return stopping_;
}

void ThreadPool::loop()
{
    while (!stopping_) {
        std::optional<std::function<void()>> f = queue_.pop();
        if (f) {
            (*f)();
        }
    }
}
