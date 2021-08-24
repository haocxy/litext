#pragma once

#include <mutex>
#include <queue>
#include <string>
#include <limits>
#include <optional>
#include <functional>
#include <condition_variable>


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
