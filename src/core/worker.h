#pragma once

#include <functional>


class Worker {
public:
    virtual ~Worker() {}

    virtual void post(std::function<void()> &&f) = 0;

    virtual bool isStopping() const = 0;
};
