#include "simple_doc.h"

#include <fstream>

#include "util/stl_container_util.h"

void SimpleDoc::LoadFromFile(const std::string &path)
{
    std::ifstream ifs(path);

    UString line;

    char c = 0;

    while (ifs.read(&c, sizeof(c)))
    {
        line.push_back(c);

        if (c == '\n')
        {
            SimpleDocLine &sl = StlContainerUtil::grow(m_lines);
            sl.setContent(std::move(line));
        }
    }

    if (!line.empty())
    {
        SimpleDocLine & sl = StlContainerUtil::grow(m_lines);
        sl.setContent(std::move(line));
    }
}
