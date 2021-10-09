#include "line_manger.h"

#include <string>

#include "core/time.h"
#include "core/logger.h"
#include "core/readable.h"


namespace doc
{

LineManager::LineManager(TextRepo &textRepo)
    : stmtSavePart_(textRepo)
{
}

LineManager::~LineManager()
{
}

void LineManager::addDocPart(const DocPart &docPart)
{
    updatePartInfo(docPart);
}

void LineManager::clear()
{
    Lock lock(mtx_);
    orderedInfos_.clear();
    pendingInfos_.clear();
    rowCount_ = 0;
}

std::map<RowN, RowIndex> LineManager::findRange(const RowRange &range) const
{
    Lock lock(mtx_);

    const RowN right = range.right();

    RowN row = range.off();

    std::map<RowN, RowIndex> foundRows;

    if (!orderedInfos_.empty()) {

        // 找到最后一个可作为结果的段落偏移
        const RowN lastUsable = std::min(right, orderedInfos_.back().rowRange().right());

        // 把可用部分取出来
        for (; row <= lastUsable; ++row) {
            foundRows[row] = *queryRowIndex(row);
        }
    }

    return foundRows;
}

Range<i64> LineManager::findByteRange(PartId partId) const
{
    Lock lock(mtx_);
    return orderedInfos_[partId].byteRange();
}

void LineManager::updatePartInfo(const DocPart &info)
{
    RowN rowCount = 0;

    std::vector<LoadProgress> progresses;

    {
        Lock lock(mtx_);

        rowCount_ += info.rowRange().count();
        rowCount = rowCount_;

        std::vector<PartId> newlyOrderedPartIds = updateRowOff(info);

        const size_t newlyOrderedPartCount = newlyOrderedPartIds.size();
        progresses.resize(newlyOrderedPartCount);

        for (size_t i = 0; i < newlyOrderedPartCount; ++i) {
            const DocPart &part = orderedInfos_[newlyOrderedPartIds[i]];
            stmtSavePart_(part);
            progresses[i] = LoadProgress(part.rowRange().end(), part.byteRange().end(), part.isLast());
        }
    }

    if (!progresses.empty()) {

        sigRowCountUpdated_(rowCount);

        for (const LoadProgress &p : progresses) {
            sigLoadProgress_(p);
        }

    }
}

std::vector<PartId> LineManager::updateRowOff(const DocPart &info)
{
    std::vector<PartId> newlyOrderedParts;

    pendingInfos_[info.byteRange().off()] = info;

    while (!pendingInfos_.empty()) {
        const DocPart &firstPending = pendingInfos_.begin()->second;
        if (firstPending.byteRange().off() == 0) {
            orderedInfos_.push_back(firstPending);
            orderedInfos_.back().setId(orderedInfos_.size() - 1);
            orderedInfos_.back().rowRange().setOff(0);
            pendingInfos_.erase(pendingInfos_.begin());
            newlyOrderedParts.push_back(orderedInfos_.back().id());
        } else {
            if (orderedInfos_.empty()) {
                break;
            } else {
                const DocPart &lastOrdered = orderedInfos_.back();
                if (orderedInfos_.back().byteRange().end() == firstPending.byteRange().off()) {
                    const RowN oldLastRowEnd = lastOrdered.rowRange().end();
                    orderedInfos_.push_back(firstPending);
                    orderedInfos_.back().setId(orderedInfos_.size() - 1);
                    orderedInfos_.back().rowRange().setOff(oldLastRowEnd);
                    pendingInfos_.erase(pendingInfos_.begin());
                    newlyOrderedParts.push_back(orderedInfos_.back().id());
                } else {
                    break;
                }
            }
        }
    }

    return newlyOrderedParts;
}

void LineManager::onRowOffDetected(DocPart &i, i64 totalByteCount)
{
    LOGD << "LineManager::onRowOffDetected"
        << ", part id: [" << i.id() << "]"
        << ", byte off: [" << i.byteRange().off() << "]"
        << ", row off: [" << i.rowRange().off() << "]"
        << ", row count: [" << i.rowRange().count() << "]"
        << ", row end: [" << i.rowRange().end() << "]";
}

std::optional<RowIndex> LineManager::queryRowIndex(RowN row) const
{
    if (orderedInfos_.empty()) {
        return std::nullopt;
    }

    if (orderedInfos_.back().rowRange().right() < row) {
        return std::nullopt;
    }

    size_t leftPartIndex = 0;
    size_t rightPartIndex = orderedInfos_.size() - 1;

    while (leftPartIndex <= rightPartIndex) {
        const size_t midPartIndex = (leftPartIndex + rightPartIndex) / 2;
        const DocPart &part = orderedInfos_[midPartIndex];

        if (row < part.rowRange().left()) {
            rightPartIndex = midPartIndex - 1;
        } else if (row > part.rowRange().right()) {
            leftPartIndex = midPartIndex + 1;
        } else {
            return RowIndex(part.id(), row - part.rowRange().off());
        }
    }

    throw std::logic_error("logic should never reach here");
}

}
