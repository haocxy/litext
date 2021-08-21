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

Signal<void(Charset)> &Document::sigCharsetDetected()
{
    return impl_->sigCharsetDetected();
}

Signal<void(const PartLoadedEvent &)> &Document::sigPartLoaded()
{
    return impl_->sigPartLoaded();
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

void Document::loadRow(RowN row, std::function<void(std::shared_ptr<Row>)> &&cb)
{
    impl_->loadRow(row, std::move(cb));
}

void Document::loadRows(const RowRange &range, std::function<void(std::vector<std::shared_ptr<Row>> &&rows)> &&cb)
{
    impl_->loadRows(range, std::move(cb));
}

}
