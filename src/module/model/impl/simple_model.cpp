#include "simple_model.h"

#include <fstream>

#include "util/stl_container_util.h"

void SimpleModel::LoadFromFile(const std::string &path)
{
    std::ifstream ifs(path);

    UString line;

    char c = 0;

    while (ifs.read(&c, sizeof(c)))
    {
        line.push_back(c);

        if (c == '\n')
        {
            SimpleLine &sl = StlContainerUtil::grow(m_lines);
            sl.setContent(std::move(line));
        }
    }

    if (!line.empty())
    {
        // TODO
        line.push_back(kDocEnd);
        SimpleLine & sl = StlContainerUtil::grow(m_lines);
        sl.setContent(std::move(line));
    }
}
