#include "io_context_strand.h"

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

IOContext *IOContextStrand::Pool::allocate(const std::string &name)
{
    std::unique_lock<std::mutex> lock(mtx_);
    return contexts_[name].get();
}
