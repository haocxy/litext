#pragma once

#include <atomic>
#include <memory_resource>

#include "core/logger.h"


class CountedMemResource : public std::pmr::memory_resource {
public:
    CountedMemResource(std::pmr::memory_resource &resource)
        : resource_(resource) {}

    virtual ~CountedMemResource() {}

    virtual void *do_allocate(std::size_t bytes, std::size_t alignment) override {
        memUsage_ += bytes;
        return resource_.allocate(bytes, alignment);
    }

    virtual void do_deallocate(void *p, std::size_t bytes, std::size_t alignment) override {
        memUsage_ -= bytes;
        resource_.deallocate(p, bytes, alignment);
    }

    virtual bool do_is_equal(const std::pmr::memory_resource &other) const noexcept override {
        return resource_.is_equal(other);
    }

    std::size_t memUsage() const {
        return memUsage_;
    }

private:
    std::pmr::memory_resource &resource_;
    std::atomic_size_t memUsage_{ 0 };
};
