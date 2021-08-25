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
    , rowCache_(textRepo_)
{
    LOGD << "Document::Document() start, path: [" << path_ << "]";

    sigConns_ += loader_.sigFatalError().connect([this](DocError e) {
        sigFatalError_(e);
    });

    sigConns_ += loader_.sigCharsetDetected().connect([this](Charset charset) {
        charset_ = charset;
        sigCharsetDetected_(charset);
    });

    sigConns_ += lineManager_.sigRowCountUpdated().connect([this](RowN nrows) {
        sigRowCountUpdated_(nrows);
    });

    sigConns_ += lineManager_.sigLoadProgress().connect([this](const LoadProgress &e) {
        sigLoadProgress_(e);
        if (e.done()) {
            LOGI << "Document loaded for [" << path_ << "] by [" << openTimeusage_.ms() << "ms]";
            sigAllLoaded_();
        }
    });

    LOGD << "Document::Document() end, path: [" << path_ << "]";
}

DocumentImpl::~DocumentImpl()
{
    LOGD << "Document::~Document(), path: [" << path_ << "]";
}

void DocumentImpl::start()
{
    LOGI << "Document start load [" << path_ << "]";

    openTimeusage_.start();

    loader_.loadAll();
}

std::optional<Row> DocumentImpl::rowAt(RowN row)
{
    std::map<RowN, Row> rows = rowsAt(Ranges::byOffAndLen<RowN>(row, 1));
    auto it = rows.find(row);
    if (it != rows.end()) {
        return std::optional<Row>(std::move(it->second));
    } else {
        return std::nullopt;
    }
}

std::map<RowN, Row> DocumentImpl::rowsAt(const RowRange &range)
{
    std::map<RowN, RowIndex> indexes = lineManager_.findRange(range);
    return rowCache_.loadRows(indexes);
}

}
