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

    sigConns_ += loader_.sigCharsetDetected().connect([this](Charset charset) {
        charset_ = charset;
        sigCharsetDetected_(charset);
    });

    sigConns_ += lineManager_.sigRowCountUpdated().connect([this](RowN nrows) {
        sigRowCountUpdated_(nrows);
    });

    sigConns_ += lineManager_.sigPartLoaded().connect([this](const PartLoadedEvent &e) {
        sigPartLoaded_(e);
    });

    LOGD << "Document::Document() end, path: [" << path_ << "]";
}

DocumentImpl::~DocumentImpl()
{
    LOGD << "Document::~Document() start, path: [" << path_ << "]";
}

void DocumentImpl::start(const RenderConfig &config)
{
    config_ = std::make_unique<RenderConfig>(config);

    lineManager_.init(config);

    loader_.loadAll();
}

void DocumentImpl::setAreaSize(int w, int h)
{
    config_->setWidthLimit(w);
    config_->setHeightLimit(h);
}

void DocumentImpl::loadRow(RowN row, std::function<void(std::shared_ptr<Row>)> &&cb)
{
    loadRows(Ranges::byOffAndLen<RowN>(row, 1), [cb = std::move(cb)](std::vector<std::shared_ptr<Row>> &&rows) {
        if (!rows.empty()) {
            cb(rows[0]);
        }
    });
}

void DocumentImpl::loadRows(const RowRange &range, std::function<void(std::vector<std::shared_ptr<Row>> &&rows)> &&cb) {

    lineManager_.loadRange(range, [this, range, cb = std::move(cb)](std::shared_ptr<std::map<RowN, RowIndex>> rowIndexes) {
        if (!rowIndexes) {
            return;
        }

        std::vector<std::shared_ptr<Row>> result;
        result.resize(range.count());

        const std::map<RowN, std::shared_ptr<Row>> foundRows = rowCache_.loadRows(*rowIndexes);

        for (RowN row = range.beg(); row < range.end(); ++row) {
            auto it = foundRows.find(row);
            if (it != foundRows.end()) {
                result[row - range.beg()] = it->second;
            }
        }

        cb(std::move(result));
    });

}

}
