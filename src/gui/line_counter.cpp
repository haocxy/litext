#include "line_counter.h"

#include "doc/document.h"


namespace gui::detail
{

LineCounter::LineCounter(StrandPool &pool, doc::Document &document)
    : worker_(pool.allocate("LineCounter"))
    , document_(document)
{

}

void LineCounter::start()
{
    auto self(shared_from_this());
    slotPartLoaded_ = document_.sigPartLoaded().connect([this, self](const doc::PartLoadedEvent &e) {
        worker_.post([this, self, e] {

        });
    });
}

void LineCounter::stop()
{
    slotPartLoaded_.disconnect();
}

}
