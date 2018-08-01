#include "simple_doc.h"

#include <fstream>

#include "util/stl_container_util.h"

#include "text/encoding_converter.h"

void SimpleDoc::LoadFromFile(const std::string &path)
{
    std::ifstream ifs(path);

    std::string line;

    char c = 0;

    while (ifs.read(&c, sizeof(c)))
    {
        line.push_back(c);

        if (c == '\n')
        {
            UString s = encoding_converter::gbkToUnicode(line);

            SimpleDocLine &sl = StlContainerUtil::grow(m_lines);
            sl.setContent(std::move(s));
        }
    }

    if (!line.empty())
    {
        SimpleDocLine & sl = StlContainerUtil::grow(m_lines);
        UString s = encoding_converter::gbkToUnicode(line);
        sl.setContent(std::move(s));
    }
}
