#include "obj_async_deleter.h"


namespace gui
{

ObjAsyncDeleter::ObjAsyncDeleter()
    : worker_([this] { loop(); })
{
}

ObjAsyncDeleter::~ObjAsyncDeleter()
{
    // 这里设置 destructing_ 不是给删除线程看的,
    // 是用来检查外部是否可以提交待删除对象
    destructing_ = true;

    // 不要 调用 stop !!!
    // 已经入队的元素应该确保被删除
    // 放入无效对象做为哨兵元素表示不会有更多的元素
    deleteTasks_.push(std::function<void()>());

    worker_.join();
}

void ObjAsyncDeleter::loop()
{
    while (true) {
        std::optional<std::function<void()>> taskOpt = deleteTasks_.pop();
        if (taskOpt) {
            std::function<void()> &task = *taskOpt;
            if (task) {
                task();
            } else {
                // 无效对象由析构函数放入,表示不再有更多元素
                break;
            }
        }
    }
}

}
