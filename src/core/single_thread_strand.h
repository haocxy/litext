#pragma once

#include <string>
#include <atomic>
#include <thread>

#include "strand.h"
#include "block_queue.h"


class SingleThreadStrand : public Strand {
public:
    SingleThreadStrand(const std::string &name);

    virtual ~SingleThreadStrand();

    virtual void post(std::function<void()> &&f) override;

    virtual bool isStopping() const override;

private:
    void loop();

private:
    const std::string name_;
    std::thread thread_;
    std::atomic_bool stopping_{ false };
    BlockQueue<std::function<void()>> queue_;
};
