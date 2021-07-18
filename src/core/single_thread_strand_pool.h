#pragma once

#include <map>
#include <mutex>
#include <memory>

#include "strand_pool.h"
#include "single_thread_strand.h"


class SingleThreadStrandPool : public StrandPool {
public:
    virtual ~SingleThreadStrandPool() {}

    virtual Strand &allocate(const std::string &name) override {
        std::unique_lock<std::mutex> lock(mtx_);
        auto &ptr = strands_[name];
        if (!ptr) {
            ptr = std::unique_ptr<SingleThreadStrand>(new SingleThreadStrand(name));
        }
        return *ptr;
    }

private:
    std::mutex mtx_;
    std::map<std::string, std::unique_ptr<SingleThreadStrand>> strands_;
};

