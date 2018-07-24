#pragma once

#include <vector>
#include <filesystem>

#include "simple_line.h"
#include "model/model.h"

class SimpleModel : public Model
{
public:
    virtual ~SimpleModel() {}

    virtual LineN lineCnt() const override
    {
        return static_cast<LineN>(m_lines.size());
    }

    virtual const Line &lineAt(LineN line) const override
    {
        return m_lines[line];
    }

public:
    void LoadFromFile(const std::string &path);

private:
    std::vector<SimpleLine> m_lines;
};
