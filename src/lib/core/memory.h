#pragma once

#include <mutex>
#include <memory_resource>

#include "core/logger.h"


class CountedMemPool : public std::pmr::memory_resource {
public:
    CountedMemPool() {}

    virtual ~CountedMemPool() {}

    virtual void *do_allocate(std::size_t bytes, std::size_t alignment) override {
        Lock lock(mtx_);
        memUsage_ += bytes;
        return resource_.allocate(bytes, alignment);
    }

    virtual void do_deallocate(void *p, std::size_t bytes, std::size_t alignment) override {
        Lock lock(mtx_);
        memUsage_ -= bytes;
        resource_.deallocate(p, bytes, alignment);
    }

    virtual bool do_is_equal(const std::pmr::memory_resource &other) const noexcept override {
        Lock lock(mtx_);
        return resource_.is_equal(other);
    }

    size_t memUsage() const {
        Lock lock(mtx_);
        return memUsage_;
    }

private:
    using Mtx = std::mutex;
    using Lock = std::lock_guard<Mtx>;
    mutable Mtx mtx_;
    std::pmr::unsynchronized_pool_resource resource_;
    size_t memUsage_ = 0;
};
