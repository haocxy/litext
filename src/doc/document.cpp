#include "document.h"

#include "core/logger.h"


namespace doc
{

Document::Document(const fs::path &path)
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

Document::~Document()
{
    LOGD << "Document::~Document() start, path: [" << path_ << "]";
}

void Document::start(const RenderConfig &config)
{
    config_ = std::make_unique<RenderConfig>(config);

    lineManager_.init(config);

    loader_.loadAll();
}

void Document::setAreaSize(int w, int h)
{
    config_->setWidthLimit(w);
    config_->setHeightLimit(h);
}

}
