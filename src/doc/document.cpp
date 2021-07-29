#include "document.h"

#include "core/logger.h"


namespace doc
{

Document::Document(const fs::path &path, const RenderConfig &config)
    : path_(path)
    , config_(config)
    , loader_(path)
    , lineManager_(loader_, config_)
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

void Document::start()
{
    loader_.loadAll();
}

}
