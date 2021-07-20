#include "document.h"

#include "core/logger.h"


namespace doc
{

namespace detail
{

Document::Document(StrandPool &pool, const fs::path &path, Strand &ownerThread)
    : path_(path)
    , ownerThread_(ownerThread)
    , textDb_(path.generic_string(), pool)
{
    LOGD << "Document::Document() start, path: [" << path_ << "]";

    sigConns_ += textDb_.sigCharsetDetected().connect([this](Charset charset) {
        auto self(shared_from_this());
        ownerThread_.post([this, self, charset] {
            charset_ = charset;
        });
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
    LOGD << "Document::~Document() path: [" << path_ << "]";
}

}

}
