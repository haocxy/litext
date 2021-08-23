#pragma once

#include "doc_define.h"


namespace doc
{

class LoadProgress {
public:
    LoadProgress() {}

    LoadProgress(RowN loadedRowCount, i64 loadedByteCount, i64 totalByteCount, bool done)
        : loadedRowCount_(loadedRowCount)
        , loadedByteCount_(loadedByteCount)
        , totalByteCount_(totalByteCount)
        , done_(done) {}

    RowN loadedRowCount() const {
        return loadedRowCount_;
    }

    i64 loadedByteCount() const {
        return loadedByteCount_;
    }

    i64 totalByteCount() const {
        return totalByteCount_;
    }

    // 是否全部加载完成
    //
    // 因为在加载过程中文档可能被其它进程修改，
    // 所以不能仅根据已加载的字节数判断是否完成加载
    bool done() const {
        return done_;
    }

private:
    RowN loadedRowCount_ = 0;
    i64 loadedByteCount_ = 0;
    i64 totalByteCount_ = 0;
    bool done_ = false;
};

}
