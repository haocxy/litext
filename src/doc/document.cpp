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

void Document::start(const RenderConfig &config)
{
    impl_->start(config);
}

void Document::setAreaSize(int w, int h)
{
    impl_->setAreaSize(w, h);
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

void Document::loadRow(RowN row, std::function<void(std::shared_ptr<Row>row)> &&cb)
{
    return impl_->loadRow(row, std::move(cb));
}

void Document::loadPage(RowN row, std::function<void(std::vector<std::shared_ptr<Row>> &&rows)> &&cb)
{
    return impl_->loadPage(row, std::move(cb));
}

}
