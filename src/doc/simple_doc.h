#pragma once

#include <vector>

#include <QString>

#include "core/fs.h"
#include "doc_row.h"
#include "doc.h"


class SimpleDoc : public Doc
{
public:
    SimpleDoc() {}

    SimpleDoc(const fs::path &file) {
        LoadFromFile(file.generic_string());
    }

    virtual ~SimpleDoc() {}

    virtual RowN rowCnt() const override
    {
        return static_cast<RowN>(m_lines.size());
    }

    virtual const Row &rowAt(RowN line) const override
    {
        return m_lines[line];
    }

public:
    bool LoadFromFile(const std::string &path);

private:
    std::vector<Row> m_lines;
};
