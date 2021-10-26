#pragma once

#include <set>
#include <mutex>
#include <shared_mutex>
#include <queue>
#include <thread>
#include <atomic>
#include <string>
#include <limits>
#include <optional>
#include <functional>
#include <condition_variable>

#include "basetype.h"


namespace ThreadUtil
{

void setNameForCurrentThread(const std::string &name);

}

template <typename T>
class BlockQueue {
private:
    static const size_t kDefaultLimit = std::numeric_limits<size_t>::max();
    using Lock = std::unique_lock<std::mutex>;

public:

    BlockQueue() {}

    BlockQueue(size_t limit)
        : limit_(limit) {}

    BlockQueue(const BlockQueue &b)
        : q_(b.q_), limit_(b.limit_) {}

    BlockQueue(BlockQueue &&b) noexcept {
        Lock lock(b.mtx_);
        q_ = std::move(b.q_);
        limit_ = b.limit_;
        b.limit_ = kDefaultLimit;
    }

    BlockQueue &operator=(const BlockQueue &b) {
        if (this != &b) {
            Lock lockThis(mtx_);
            Lock lockThat(b.mtx_);
            q_ = b.q_;
            limit_ = b.limit_;
        }
        return *this;
    }

    BlockQueue &operator=(BlockQueue &b) {
        if (this != &b) {
            Lock lockThis(mtx_);
            Lock lockThat(b.mtx_);
            q_ = std::move(b.q_);
            limit_ = b.limit_;
            b.limit_ = kDefaultLimit;
        }
        return *this;
    }

    i32 size() const {
        Lock lock(mtx_);
        return static_cast<i32>(q_.size());
    }

    void stop() {
        Lock lock(mtx_);
        stopping_ = true;
        cvhasCapa_.notify_all();
        cvhasElem_.notify_all();
    }

    bool push(const T &e) {
        Lock lock(mtx_);
        if (stopping_) {
            return false;
        }
        while (q_.size() >= limit_) {
            cvhasCapa_.wait(lock);
            if (stopping_) {
                return false;
            }
        }
        q_.push(e);
        cvhasElem_.notify_one();
        return true;
    }

    bool push(T &&e) {
        Lock lock(mtx_);
        if (stopping_) {
            return false;
        }
        while (q_.size() >= limit_) {
            cvhasCapa_.wait(lock);
            if (stopping_) {
                return false;
            }
        }
        q_.push(std::move(e));
        cvhasElem_.notify_one();
        return true;
    }

    std::optional<T> pop() {
        Lock lock(mtx_);
        if (stopping_) {
            return std::nullopt;
        }
        while (q_.empty()) {
            cvhasElem_.wait(lock);
            if (stopping_) {
                return std::nullopt;
            }
        }
        T e(std::move(q_.front()));
        q_.pop();
        cvhasCapa_.notify_one();
        return e;
    }

private:
    mutable std::mutex mtx_;
    mutable std::condition_variable cvhasElem_;
    mutable std::condition_variable cvhasCapa_;
    bool stopping_ = false;
    std::queue<T> q_;
    size_t limit_ = kDefaultLimit;
};

template <typename F>
using TaskQueue = BlockQueue<std::function<F>>;


class StrandEntry {
public:
    using Task = std::function<void()>;

    virtual ~StrandEntry() {}

    virtual void exec(Task &&task) {
        if (inThread()) {
            task();
        } else {
            post(std::move(task));
        }
    }

    virtual void post(Task &&task) = 0;

protected:
    virtual bool inThread() const = 0;
};


class StrandAllocator {
public:
    virtual ~StrandAllocator() {}

    virtual StrandEntry *allocate() = 0;
};


class Strand {
public:
    using Task = StrandEntry::Task;

    Strand(StrandAllocator &strandAllocator)
        : entry_(strandAllocator.allocate()) {}

    void stop() {
        entry_ = nullptr;
    }

    void exec(Task &&task) {
        entry_->exec(std::move(task));
    }

    void post(Task &&task) {
        entry_->post(std::move(task));
    }

private:
    uptr<StrandEntry> entry_;
};


class StrandPool : public StrandAllocator {
private:

    class TaskPack {
    public:
        TaskPack(void *delegatePointer, StrandEntry::Task &&task)
            : delegatePointer_(delegatePointer), task_(std::move(task)) {}

        void *delegatePointer() {
            return delegatePointer_;
        }

        StrandEntry::Task &task() {
            return task_;
        }

    private:
        void *delegatePointer_;
        StrandEntry::Task task_;
    };

    class Worker {
    public:
        Worker(const std::string &name, StrandPool &strandPool);

        ~Worker();

        i32 queueSize() const {
            return taskQueue_.size();
        }

        bool inThread() const {
            return std::this_thread::get_id() == thread_.get_id();
        }

        void post(void *delegatePointer, StrandEntry::Task &&task) {
            taskQueue_.push(TaskPack(delegatePointer, std::move(task)));
        }

    private:
        void threadBody();

    private:
        StrandPool &strandPool_;
        std::atomic_bool &stopping_;
        
        BlockQueue<TaskPack> taskQueue_;
        std::thread thread_;
    };

public:
    StrandPool(const std::string &name, i32 threadCount);

    virtual ~StrandPool();

    virtual StrandEntry *allocate();

    class DelegateStrand : public StrandEntry {
    public:
        DelegateStrand(StrandPool &strandPool, Worker &worker);

        virtual ~DelegateStrand();

        virtual void post(StrandEntry::Task &&task) {
            worker_.post(this, std::move(task));
        }

    protected:
        virtual bool inThread() const {
            return worker_.inThread();
        }

    private:
        StrandPool &strandPool_;
        Worker &worker_;
    };

private:
    void initWorkers(const std::string &name, i32 threadCount);

    Worker &chooseWorker();

    bool isStrandLiving(DelegateStrand *delegateStrand);

    void insertLivingDelegate(DelegateStrand *delegateStrand);

    void removeLivingDelegate(DelegateStrand *delegateStrand);

private:
    using MtxForLivingDelegates = std::shared_mutex;
    using WriteLockForLivingDelegates = std::lock_guard<MtxForLivingDelegates>;
    using ReadLockForLivingDelegates = std::shared_lock<MtxForLivingDelegates>;
    mutable MtxForLivingDelegates mtxForLivingDelegates_;
    std::set<DelegateStrand *> livingDelegates_;

    std::atomic_bool stopping_{ false };

    using MtxForWorkers = std::mutex;
    using LockForWorkers = std::lock_guard<MtxForWorkers>;
    mutable MtxForWorkers mtxForWorkers_;

    std::vector<uptr<Worker>> workers_;
};


class SingleThreadStrand : public StrandEntry {
public:
    SingleThreadStrand(const std::string &name);

    virtual ~SingleThreadStrand();

protected:
    virtual bool inThread() const override;

    virtual void post(Task &&task) override;

private:
    void threadBody();

private:
    std::atomic_bool stopping_{ false };
    std::thread thread_;
    BlockQueue<Task> queue_;
};
