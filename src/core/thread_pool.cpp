#include "thread_pool.h"

#include <sstream>

#include "core/thread_util.h"
#include "core/logger.h"


ThreadPool::ThreadPool(const std::string &name, int threadCount)
    : name_(name)
{
    for (int i = 0; i < threadCount; ++i) {
        threads_.push_back(std::thread([this, i]() {
            std::ostringstream ss;
            ss << name_ << "(" << i << ")";
            std::string threadName = ss.str();
            ThreadUtil::setNameForCurrentThread(threadName);
            LOGI << "Thread in ThreadPool named [" << threadName << "] started";
            loop();
            LOGI << "Thread in ThreadPool named [" << threadName << "] stopped";
        }));
    }
}

ThreadPool::~ThreadPool()
{
    stop();
}

void ThreadPool::post(std::function<void()> &&f)
{
    queue_.push(std::move(f));
}

bool ThreadPool::isStopping() const
{
    return stopping_;
}

void ThreadPool::stop()
{
    stopping_ = true;
    queue_.stop();
    for (std::thread &t : threads_) {
        t.join();
    }
    threads_.clear();
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
