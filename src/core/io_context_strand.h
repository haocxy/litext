#pragma once

#include <map>
#include <string>
#include <mutex>
#include <memory>

#include "strand.h"
#include "io_context.h"


class IOContextStrand : public Strand {
public:

    class Pool {
    public:
        Pool() {}

        IOContext *allocate(const std::string &name);

    private:
        std::mutex mtx_;
        std::map<std::string, std::unique_ptr<IOContext>> contexts_;
    };

    IOContextStrand(Pool &pool, const std::string &name);

    virtual ~IOContextStrand();

    virtual void post(std::function<void()> &&f) override;

private:
    // 析构时不对这个指针delete
    IOContext *context_ = nullptr;
};
