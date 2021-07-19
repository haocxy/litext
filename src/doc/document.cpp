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

void Document::init()
{
    textDbSlotCharsetDetected_ = textDb_.sigCharsetDetected().connect([this](Charset charset) {
        auto self(shared_from_this());
        ownerThread_.post([this, self, charset] {
            this->charset_ = charset;
        });
        sigCharsetDetected_(charset);
    });

    textDbSlotPartLoaded_ = textDb_.sigPartLoaded().connect(sigPartLoaded_);

    textDbSlotAllLoaded_ = textDb_.sigAllLoaded().connect(sigAllLoaded_);
}

}

}
