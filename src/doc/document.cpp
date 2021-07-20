#include "document.h"


namespace doc
{

namespace detail
{

Document::Document(StrandPool &pool, const fs::path &path, Strand &ownerThread)
    : path_(path)
    , ownerThread_(ownerThread)
    , textDb_(path.generic_string(), pool)
{

}

Document::~Document()
{

}

void Document::start()
{
    auto self(shared_from_this());

    textDbSlotCharsetDetected_ = textDb_.sigCharsetDetected().connect([this, self](Charset charset) {
        ownerThread_.post([this, self, charset] {
            charset_ = charset;
        });
        sigCharsetDetected_(charset);
    });

    textDbSlotPartLoaded_ = textDb_.sigPartLoaded().connect([this, self](const PartLoadedEvent &e) {
        sigPartLoaded_(e);
    });

    textDbSlotAllLoaded_ = textDb_.sigAllLoaded().connect([this, self] {
        sigAllLoaded_();
    });
}

void Document::stop()
{
    textDbSlotAllLoaded_.disconnect();
    textDbSlotPartLoaded_.disconnect();
    textDbSlotCharsetDetected_.disconnect();
}

}

}
