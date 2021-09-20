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
    delete impl_;
    impl_ = nullptr;
}

const fs::path &Document::path() const
{
    return impl_->path();
}

i64 Document::loadTimeUsageMs() const
{
    return impl_->loadTimeUsageMs();
}

void Document::start()
{
    impl_->start();
}

Signal<void(DocError)> &Document::sigFatalError()
{
    return impl_->sigFatalError();
}

Signal<void(i64)> &Document::sigFileSizeDetected()
{
    return impl_->sigFileSizeDetected();
}

Signal<void(Charset)> &Document::sigCharsetDetected()
{
    return impl_->sigCharsetDetected();
}

Signal<void(const LoadProgress &)> &Document::sigLoadProgress()
{
    return impl_->sigLoadProgress();
}

Signal<void()> &Document::sigAllLoaded()
{
    return impl_->sigAllLoaded();
}

Signal<void(RowN)> &Document::sigRowCountUpdated()
{
    return impl_->sigRowCountUpdated();
}

RowN Document::rowCnt() const
{
    return impl_->rowCnt();
}

sptr<Row> Document::rowAt(RowN row) const
{
    return impl_->rowAt(row);
}

std::map<RowN, sptr<Row>> Document::rowsAt(const RowRange &range) const
{
    return impl_->rowsAt(range);
}

}
