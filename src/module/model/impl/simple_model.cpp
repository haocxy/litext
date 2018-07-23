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
        SimpleLine & sl = StlContainerUtil::grow(m_lines);
        sl.setContent(std::move(line));
    }

    // 如果不是以换行结尾的，则增加一个哨兵字符，以简化文档结尾的处理
    if (m_lines.empty())
    {
        SimpleLine & sl = StlContainerUtil::grow(m_lines);
        sl.Add(0);
    }
    else
    {
        SimpleLine &sl = m_lines.back();
        const CharN cnt = sl.charCnt();
        if (cnt == 0)
        {
            sl.Add(0);
        }
        else
        {
            const UChar last = sl.charAt(cnt - 1);
            if (last != '\r' && last != '\n')
            {
                sl.Add(0);
            }
        }
    }
}
