#include "document_impl.h"

#include "core/logger.h"
#include "core/system.h"

#include "dbfiles.h"


namespace doc
{

DocumentImpl::DocumentImpl(StrandAllocator &strandAllocator, AsyncDeleter &asyncDeleter, const fs::path &path)
    : strand_(strandAllocator)
    , asyncDeleter_(asyncDeleter)
    , path_(path)
    , textRepo_(dbfiles::docPathToDbPath(path))
    , lineManager_(textRepo_)
    , rowCache_(lineManager_, path)
{
    LOGD << "Document::Document() constructing, path: [" << path_ << "]";

    lineSigConns_ += lineManager_.sigRowCountUpdated().connect([this](RowN nrows) {
        strand_.exec([this, nrows] {
            sigRowCountUpdated_(nrows);
        });
    });

    lineSigConns_ += lineManager_.sigLoadProgress().connect([this](const LoadProgress &e) {
        strand_.exec([this, e] {

            sigLoadProgress_(e);

            if (e.done()) {
                {
                    //WriteLock lock(mtxForLoad_);
                    _asyncDeleteLoader();
                    loadTimeUsage_.stop();
                    const i64 usageMs = loadTimeUsage_.ms();
                    LOGI << "Document [" << path_ << "] loaded by [" << usageMs << " ms]";
                }

                sigAllLoaded_();
            }
        });
    });

    LOGD << "Document::Document() constructed, path: [" << path_ << "]";
}

DocumentImpl::~DocumentImpl()
{
    strand_.stop();

    loadSigConns_.clear();
    lineSigConns_.clear();
    loader_ = nullptr;

    LOGD << "Document::~Document() path: [" << path_ << "]";
}

void DocumentImpl::asyncLoad(Charset charset)
{
    strand_.exec([this, charset] {

        const bool isInitLoad = isInitLoad_;

        isInitLoad_ = false;

        loadSigConns_.clear();

        LOGI << "Document start load [" << path_ << "]";

        _asyncDeleteLoader();

        TextRepo::ClearPartsStmt clearPartsStmt(textRepo_);
        clearPartsStmt();

        lineManager_.clear();
        rowCache_.clear();

        loader_ = std::make_unique<TextLoader>(path_);

        _bindLoadSignals(loadSigConns_, *loader_);

        loadTimeUsage_.start();

        loader_->loadAll(charset);


        sigStartLoad_(Document::StartLoadEvent(isInitLoad));
    });
}

sptr<Row> DocumentImpl::rowAt(RowN row) const
{
    std::map<RowN, sptr<Row>> rows = _rowsAt(Ranges::byOffAndLen<RowN>(row, 1));
    auto it = rows.find(row);
    if (it != rows.end()) {
        return it->second;
    } else {
        return nullptr;
    }
}

std::map<RowN, sptr<Row>> DocumentImpl::rowsAt(const RowRange &range) const
{
    return _rowsAt(range);
}

std::map<RowN, sptr<Row>> DocumentImpl::_rowsAt(const RowRange &range) const
{
    std::map<RowN, RowIndex> indexes = lineManager_.findRange(range);
    return rowCache_.loadRows(indexes);
}

void DocumentImpl::_bindLoadSignals(SigConns &conns, TextLoader &loader)
{
    conns += loader.sigPartLoaded().connect([this](const DocPart &part) {
        strand_.exec([this, part] {
            lineManager_.addDocPart(part);
        });
    });

    conns += loader.sigFatalError().connect([this](DocError e) {
        strand_.exec([this, e] {
            sigFatalError_(e);
        });
    });

    conns += loader.sigFileSizeDetected().connect([this](i64 fileSize) {
        strand_.exec([this, fileSize] {
            fileSize_ = fileSize;
            sigFileSizeDetected_(fileSize);
        });
    });

    conns += loader.sigCharsetDetected().connect([this](Charset charset) {
        strand_.exec([this, charset] {
            charset_ = charset;
            rowCache_.updateCharset(charset);
            sigCharsetDetected_(charset);
        });
    });
}

void DocumentImpl::_deleteLoader()
{
    if (loader_) {
        loadSigConns_.clear();
        loader_ = nullptr;
    }
}

void DocumentImpl::_asyncDeleteLoader()
{
    if (loader_) {
        loadSigConns_.clear();
        TextLoader *loader = loader_.release();
        asyncDeleter_.asyncDelete([loader] { delete loader; });
        loader = nullptr;
    }
}

}
