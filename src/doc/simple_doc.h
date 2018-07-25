#pragma once

#include <vector>
#include <filesystem>

#include "simple_doc_line.h"
#include "doc.h"

class SimpleDoc : public Doc
{
public:
    virtual ~SimpleDoc() {}

    virtual LineN lineCnt() const override
    {
        return static_cast<LineN>(m_lines.size());
    }

    virtual const DocLine &lineAt(LineN line) const override
    {
        return m_lines[line];
    }

public:
    void LoadFromFile(const std::string &path);

private:
    std::vector<SimpleDocLine> m_lines;
};
