#pragma once

#include <vector>
#include <filesystem>

#include "simple_doc_row.h"
#include "doc.h"

class SimpleDoc : public Doc
{
public:
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
    void LoadFromFile(const std::string &path);

private:
    std::vector<SimpleRow> m_lines;
};
