#pragma once

#include "core/fs.h"

#include "doc_define.h"


namespace doc
{

class OpenInfo {
public:
    OpenInfo() {}

    OpenInfo(const fs::path &file)
        : file_(file) {}

    OpenInfo(const fs::path &file, RowN row)
        : file_(file)
        , row_(row) {}

    const fs::path &file() const {
        return file_;
    }

    RowN row() const {
        return row_;
    }

private:
    fs::path file_;
    RowN row_ = 0;
};

}
