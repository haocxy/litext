#include "thread.h"

#include <cassert>
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

SingleThreadStrand::SingleThreadStrand(const std::string &name)
{
    thread_ = std::thread([this, name] {
        ThreadUtil::setNameForCurrentThread(name);
        threadBody();
    });
}

SingleThreadStrand::~SingleThreadStrand()
{
    stopping_ = true;
    thread_.join();
}

bool SingleThreadStrand::inThread() const
{
    return std::this_thread::get_id() == thread_.get_id();
}

void SingleThreadStrand::post(Task &&task)
{
    if (!stopping_) {
        queue_.push(std::move(task));
    }
}

void SingleThreadStrand::threadBody()
{
    while (!stopping_) {
        std::optional<Task> &&task = queue_.pop();
        if (task && *task) {
            (*task)();
        }
    }
}

StrandPool::DelegateStrand::DelegateStrand(StrandPool &strandPool, Worker &worker)
    : strandPool_(strandPool), worker_(worker)
{
    strandPool_.insertLivingDelegate(this);
}

StrandPool::DelegateStrand::~DelegateStrand()
{
    strandPool_.removeLivingDelegate(this);
}

StrandPool::StrandPool(const std::string &name, i32 threadCount)
{
    initWorkers(name, threadCount);
}

StrandPool::~StrandPool()
{
    stopping_ = true;
    workers_.clear();
}

StrandEntry *StrandPool::allocate()
{
    Worker &worker = chooseWorker();

    return new DelegateStrand(*this, worker);
}

void StrandPool::initWorkers(const std::string &name, i32 threadCount)
{
    LockForWorkers lock(mtxForWorkers_);

    for (i32 i = 0; i < threadCount; ++i) {
        workers_.push_back(std::make_unique<Worker>(name, *this));
    }
}

StrandPool::Worker &StrandPool::chooseWorker()
{
    LockForWorkers lock(mtxForWorkers_);

    auto beg = workers_.begin();
    auto end = workers_.end();

    if (beg == end) {
        throw std::logic_error("StrandPool::chooseWorker(): no worker");
    }

    auto best = beg;

    i32 minQueueSize = (*best)->queueSize();

    for (auto it = beg + 1; it != end; ++it) {
        const i32 queueSize = (*it)->queueSize();
        if (queueSize < minQueueSize) {
            minQueueSize = queueSize;
            best = it;
        }
    }

    return **best;
}

bool StrandPool::isStrandLiving(DelegateStrand *delegateStrand)
{
    ReadLockForLivingDelegates lock(mtxForLivingDelegates_);

    return livingDelegates_.find(delegateStrand) != livingDelegates_.end();
}

void StrandPool::insertLivingDelegate(DelegateStrand *delegateStrand)
{
    WriteLockForLivingDelegates lock(mtxForLivingDelegates_);

    livingDelegates_.insert(delegateStrand);
}

void StrandPool::removeLivingDelegate(DelegateStrand *delegateStrand)
{
    WriteLockForLivingDelegates lock(mtxForLivingDelegates_);

    livingDelegates_.erase(delegateStrand);
}

StrandPool::Worker::Worker(const std::string &name, StrandPool &strandPool)
    : strandPool_(strandPool), stopping_(strandPool.stopping_)
{
    thread_ = std::thread([this, name] {
        ThreadUtil::setNameForCurrentThread(name);
        threadBody();
    });
}

StrandPool::Worker::~Worker()
{
    taskQueue_.stop();
    thread_.join();
}

void StrandPool::Worker::threadBody()
{
    while (!stopping_) {
        std::optional<TaskPack> &&taskPack = taskQueue_.pop();
        if (taskPack) {
            DelegateStrand *delegatePointer = reinterpret_cast<DelegateStrand *>(taskPack->delegatePointer());
            assert(delegatePointer);
            if (strandPool_.isStrandLiving(delegatePointer)) {
                StrandEntry::Task &task = taskPack->task();
                if (task) {
                    task();
                }
            }
        }
    }
}
