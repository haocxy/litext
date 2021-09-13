#include "document_impl.h"

#include "core/logger.h"
#include "core/system.h"

#include "dbfiles.h"


namespace doc
{

DocumentImpl::DocumentImpl(const fs::path &path)
    : path_(path)
    , textRepo_(dbfiles::docPathToDbPath(path))
    , loader_(path)
    , lineManager_(textRepo_, loader_)
    , rowCache_(lineManager_, path)
{
    LOGD << "Document::Document() start, path: [" << path_ << "]";

    sigConns_ += loader_.sigFatalError().connect([this](DocError e) {
        sigFatalError_(e);
    });

    sigConns_ += loader_.sigCharsetDetected().connect([this](Charset charset) {
        charset_ = charset;
        rowCache_.updateCharset(charset);
        sigCharsetDetected_(charset);
    });

    sigConns_ += lineManager_.sigRowCountUpdated().connect([this](RowN nrows) {
        sigRowCountUpdated_(nrows);
    });

    sigConns_ += lineManager_.sigLoadProgress().connect([this](const LoadProgress &e) {
        sigLoadProgress_(e);
        if (e.done()) {
            loadTimeUsage_.stop();
            const i64 usageMs = loadTimeUsage_.ms();
            LOGI << "Document [" << path_ << "] loaded by [" << usageMs << " ms]";
            sigAllLoaded_();
        }
    });

    LOGD << "Document::Document() end, path: [" << path_ << "]";
}

DocumentImpl::~DocumentImpl()
{
    sigConns_.clear();

    LOGD << "Document::~Document(), path: [" << path_ << "]";
}

void DocumentImpl::start()
{
    LOGI << "Document start load [" << path_ << "]";

    loadTimeUsage_.start();

    loader_.loadAll();
}

sptr<Row> DocumentImpl::rowAt(RowN row) const
{
    std::map<RowN, sptr<Row>> rows = rowsAt(Ranges::byOffAndLen<RowN>(row, 1));
    auto it = rows.find(row);
    if (it != rows.end()) {
        return it->second;
    } else {
        return nullptr;
    }
}

std::map<RowN, sptr<Row>> DocumentImpl::rowsAt(const RowRange &range) const
{
    std::map<RowN, RowIndex> indexes = lineManager_.findRange(range);
    return rowCache_.loadRows(indexes);
}

}
