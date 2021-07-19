#include "document.h"


namespace doc
{

Document::Document(StrandPool &pool, const fs::path &path, Strand &ownerThread)
    : path_(path)
    , ownerThread_(ownerThread)
    , textDb_(path.generic_string(), pool)
{
    slotTextDatabaseAllLoaded_ = textDb_.sigAllLoaded().connect(sigAllLoaded_);
}

Document::~Document()
{

}

}
