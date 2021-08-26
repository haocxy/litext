#include "document.h"

#include "document_impl.h"


namespace doc
{



Document::Document(const fs::path &file)
    : impl_(new DocumentImpl(file))
{
}

Document::~Document()
{
    delete impl_;
    impl_ = nullptr;
}

void Document::start()
{
    impl_->start();
}

Signal<void(DocError)> &Document::sigFatalError()
{
    return impl_->sigFatalError();
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

std::optional<Row> Document::rowAt(RowN row)
{
    return impl_->rowAt(row);
}

std::map<RowN, Row> Document::rowsAt(const RowRange &range)
{
    return impl_->rowsAt(range);
}

}