#include "document.h"

#include <iostream>
#include <future>

#include "core/logger.h"
#include "core/time_util.h"
#include "core/system_util.h"
#include "core/readable_size_util.h"
#include "core/charset_converter.h"

#include "sql/asset.prepare_db.sql.h"


namespace doc::detail
{

DocumentImpl::DocumentImpl(IOContextStrand::Pool &pool, const fs::path &path)
    : path_(path)
    , textDb_(path.generic_string(), pool)
{

}

DocumentImpl::~DocumentImpl()
{

}

bool DocumentImpl::loaded() const
{
    return loaded_;
}

RowN DocumentImpl::loadedRowCount() const
{
    return loadedRowCount_;
}

Charset DocumentImpl::charset() const
{
    return charset_;
}

}
