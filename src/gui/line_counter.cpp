#include "line_counter.h"

#include "doc/document.h"


namespace gui::detail
{

LineCounter::LineCounter(StrandPool &pool, doc::Document &document)
    : worker_(pool.allocate("LineCounter"))
    , document_(document)
{
    slotPartLoaded_ = document_.sigPartLoaded().connect([this](const doc::PartLoadedEvent &progress) {

    });
}

}
