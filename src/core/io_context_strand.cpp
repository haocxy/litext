#include "io_context_strand.h"

#include "logger.h"


IOContextStrand::IOContextStrand(Pool &pool, const std::string &name)
    : context_(pool.allocate(name))
{

}

IOContextStrand::~IOContextStrand()
{
    context_ = nullptr;
}

void IOContextStrand::post(std::function<void()> &&f)
{
    if (context_) {
        context_->post(std::move(f));
    }
}

IOContextStrand::Pool::~Pool()
{

}

IOContext *IOContextStrand::Pool::allocate(const std::string &name)
{
    std::unique_lock<std::mutex> lock(mtx_);
    auto it = contexts_.find(name);
    if (it != contexts_.end()) {
        return it->second.get();
    } else {
        std::unique_ptr<IOContext> &ptr = contexts_[name];
        ptr = std::unique_ptr<IOContext>(new IOContext(name));
        return ptr.get();
    }
}
