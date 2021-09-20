#pragma once

#include <functional>

class AsyncDeleter {
public:
    virtual ~AsyncDeleter() {}

    virtual void asyncDelete(std::function<void()> &&deleteAction) = 0;
};
