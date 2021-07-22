#include "document.h"

#include "core/logger.h"


namespace doc
{

namespace detail
{

Document::Document(const fs::path &path)
    : path_(path)
    , textDb_(path)
{
    LOGD << "Document::Document() start, path: [" << path_ << "]";

    sigConns_ += textDb_.sigCharsetDetected().connect([this](Charset charset) {
        charset_ = charset;
        sigCharsetDetected_(charset);
    });

    sigConns_ += textDb_.sigPartLoaded().connect([this](const PartLoadedEvent &e) {
        sigPartLoaded_(e);
    });

    sigConns_ += textDb_.sigAllLoaded().connect([this] {
        sigAllLoaded_();
    });

    LOGD << "Document::Document() end, path: [" << path_ << "]";
}

Document::~Document()
{
    LOGD << "Document::~Document() start, path: [" << path_ << "]";
}

void Document::start()
{
    textDb_.start();
}

}

}
