#pragma once

#include <atomic>
#include <thread>
#include <memory>

#include "core/thread.h"
#include "core/async_deleter.h"


namespace gui
{

class ObjAsyncDeleter : public AsyncDeleter {
public:
    ObjAsyncDeleter();

    virtual ~ObjAsyncDeleter();

    virtual void asyncDelete(std::function<void()> &&deleteTask) override {
        if (!destructing_) {
            deleteTasks_.push(std::move(deleteTask));
        } else {
            // destructing_ 由析构函数置为 true, 显然在析构函数后就不应该继续使用了, 这是个BUG
            throw std::logic_error("ObjAsyncDeleter already destructed, logic has bug");
        }
    }

private:
    void loop();

private:

    // 由析构函数置为 true
    std::atomic_bool destructing_{ false };

    // TODO 应该多开几个线程,确保析构逻辑尽快结束
    // 因为在正常逻辑中析构逻辑的结束是必须的
    std::thread worker_;

    TaskQueue<void()> deleteTasks_;
};

}
