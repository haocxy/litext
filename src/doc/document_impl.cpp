#include "document_impl.h"

#include "core/logger.h"


namespace doc
{

DocumentImpl::DocumentImpl(const fs::path &path)
    : path_(path)
    , textRepo_(path.generic_string() + ".notesharpdb")
    , loader_(path)
    , lineManager_(textRepo_, loader_)
{
    LOGD << "Document::Document() start, path: [" << path_ << "]";

    sigConns_ += loader_.sigCharsetDetected().connect([this](Charset charset) {
        charset_ = charset;
        sigCharsetDetected_(charset);
    });

    sigConns_ += lineManager_.sigRowCountUpdated().connect([this](RowN nrows) {
        sigRowCountUpdated_(nrows);
    });

    sigConns_ += lineManager_.sigPartLoaded().connect([this](const PartLoadedEvent &e) {
        sigPartLoaded_(e);
    });

    LOGD << "Document::Document() end, path: [" << path_ << "]";
}

DocumentImpl::~DocumentImpl()
{
    LOGD << "Document::~Document() start, path: [" << path_ << "]";
}

void DocumentImpl::start(const RenderConfig &config)
{
    config_ = std::make_unique<RenderConfig>(config);

    lineManager_.init(config);

    loader_.loadAll();
}

void DocumentImpl::setAreaSize(int w, int h)
{
    config_->setWidthLimit(w);
    config_->setHeightLimit(h);
}

void DocumentImpl::loadRow(RowN row, std::function<void(std::shared_ptr<Row>row)> &&cb) {
    // TODO test code
    lineManager_.loadRange(row, 3000, std::function<void(LineManager::LoadRangeResult)>());
}

void DocumentImpl::loadPage(RowN row, std::function<void(std::vector<std::shared_ptr<Row>> &&rows)> &&cb) {
    // TODO test code
    lineManager_.loadRange(row, 3000, std::function<void(LineManager::LoadRangeResult)>());
}

}
