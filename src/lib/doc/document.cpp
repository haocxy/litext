#include "document.h"

#include "document_impl.h"


namespace doc
{



Document::Document(AsyncDeleter &asyncDeleter, const fs::path &file)
    : impl_(new DocumentImpl(asyncDeleter, file))
{
}

Document::~Document()
{
    WriteLock lock(mtx_);
    delete impl_;
    impl_ = nullptr;
}

const fs::path &Document::path() const
{
    ReadLock lock(mtx_);
    return impl_->path();
}

i64 Document::loadTimeUsageMs() const
{
    ReadLock lock(mtx_);
    return impl_->loadTimeUsageMs();
}

void Document::load(Charset charset)
{
    ReadLock lock(mtx_);
    impl_->load(charset);
}

Signal<void(DocError)> &Document::sigFatalError()
{
    ReadLock lock(mtx_);
    return impl_->sigFatalError();
}

Signal<void(const Document::StartLoadEvent &)> &Document::sigStartLoad()
{
    ReadLock lock(mtx_);
    return impl_->sigStartLoad();
}

Signal<void(i64)> &Document::sigFileSizeDetected()
{
    ReadLock lock(mtx_);
    return impl_->sigFileSizeDetected();
}

Signal<void(Charset)> &Document::sigCharsetDetected()
{
    ReadLock lock(mtx_);
    return impl_->sigCharsetDetected();
}

Signal<void(const LoadProgress &)> &Document::sigLoadProgress()
{
    ReadLock lock(mtx_);
    return impl_->sigLoadProgress();
}

Signal<void()> &Document::sigAllLoaded()
{
    ReadLock lock(mtx_);
    return impl_->sigAllLoaded();
}

Signal<void(RowN)> &Document::sigRowCountUpdated()
{
    ReadLock lock(mtx_);
    return impl_->sigRowCountUpdated();
}

RowN Document::rowCnt() const
{
    ReadLock lock(mtx_);
    return impl_->rowCnt();
}

sptr<Row> Document::rowAt(RowN row) const
{
    ReadLock lock(mtx_);
    return impl_->rowAt(row);
}

std::map<RowN, sptr<Row>> Document::rowsAt(const RowRange &range) const
{
    ReadLock lock(mtx_);
    return impl_->rowsAt(range);
}

}
