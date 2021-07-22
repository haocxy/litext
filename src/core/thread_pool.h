#pragma once

#include <string>
#include <atomic>
#include <thread>
#include <vector>

#include "worker.h"
#include "block_queue.h"


class ThreadPool : public Worker {
public:
    ThreadPool(const std::string &name, int threadCount);

    virtual ~ThreadPool();

    virtual void post(std::function<void()> &&f) override;

    virtual bool isStopping() const override;

private:
    void loop();

private:
    const std::string name_;
    std::vector<std::thread> threads_;
    std::atomic_bool stopping_{ false };
    BlockQueue<std::function<void()>> queue_;
};
