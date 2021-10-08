#include "document_impl.h"

#include "core/logger.h"
#include "core/system.h"

#include "dbfiles.h"


namespace doc
{

DocumentImpl::DocumentImpl(AsyncDeleter &asyncDeleter, const fs::path &path)
    : asyncDeleter_(asyncDeleter)
    , path_(path)
    , textRepo_(dbfiles::docPathToDbPath(path))
{
    LOGD << "Document::Document() start, path: [" << path_ << "]";



    LOGD << "Document::Document() end, path: [" << path_ << "]";
}

DocumentImpl::~DocumentImpl()
{
    loadSigConns_.clear();
    lineSigConns_.clear();
    loader_ = nullptr;

    LOGD << "Document::~Document(), path: [" << path_ << "]";
}

void DocumentImpl::load(Charset charset)
{
    LOGI << "Document start load [" << path_ << "]";

    TextRepo::ClearPartsStmt clearPartsStmt(textRepo_);
    clearPartsStmt();

    deleteLoader();

    loadSigConns_.clear();
    lineSigConns_.clear();

    

    lineManager_ = std::make_unique<LineManager>(textRepo_);

    lineSigConns_ += lineManager_->sigRowCountUpdated().connect([this](RowN nrows) {
        sigRowCountUpdated_(nrows);
    });

    lineSigConns_ += lineManager_->sigLoadProgress().connect([this](const LoadProgress &e) {
        sigLoadProgress_(e);
        if (e.done()) {
            loadTimeUsage_.stop();
            const i64 usageMs = loadTimeUsage_.ms();
            LOGI << "Document [" << path_ << "] loaded by [" << usageMs << " ms]";
            sigAllLoaded_();

            deleteLoader();
        }
    });

    rowCache_ = std::make_unique<RowCache>(*lineManager_, path_);

    loader_ = std::make_unique<TextLoader>(path_);

    bindLoadSignals(loadSigConns_, *loader_);

    sigStartLoad_();

    loadTimeUsage_.start();

    loader_->loadAll(charset);
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
    std::map<RowN, RowIndex> indexes = lineManager_->findRange(range);
    return rowCache_->loadRows(indexes);
}

void DocumentImpl::bindLoadSignals(SigConns &conns, TextLoader &loader)
{
    conns += loader.sigPartLoaded().connect([this](const DocPart &part) {
        lineManager_->addDocPart(part);
    });

    conns += loader.sigFatalError().connect([this](DocError e) {
        sigFatalError_(e);
    });

    conns += loader.sigFileSizeDetected().connect([this](i64 fileSize) {
        fileSize_ = fileSize;
        sigFileSizeDetected_(fileSize);
    });

    conns += loader.sigCharsetDetected().connect([this](Charset charset) {
        charset_ = charset;
        rowCache_->updateCharset(charset);
        sigCharsetDetected_(charset);
    });
}

void DocumentImpl::deleteLoader()
{
    if (loader_) {
        loadSigConns_.clear();
        TextLoader *loader = loader_.release();
        asyncDeleter_.asyncDelete([loader] { delete loader; });
        loader = nullptr;
    }
}

}
