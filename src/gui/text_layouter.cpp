#include "text_layouter.h"

#include "doc/document.h"


namespace gui::detail
{

TextLayouterImpl::TextLayouterImpl(StrandPool &pool, doc::Document &document)
    : worker_(pool.allocate("LineCounter"))
    , document_(document)
{
    sigConns_ += document_.sigPartLoaded().connect([this](const doc::PartLoadedEvent &e) {
        auto self(shared_from_this());
        worker_.post([this, self, e] {

        });
    });
}

}
