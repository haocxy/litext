#include "simple_doc.h"

#include <fstream>

#include "util/stl_container_util.h"

#include "text/encoding_converter.h"

void SimpleDoc::LoadFromFile(const std::string &path)
{
    namespace scu = StlContainerUtil;

    std::ifstream ifs(path);

    std::string buff;

    char c = 0;

    while (ifs.read(&c, sizeof(c)))
    {
        buff.push_back(c);

        if (c == '\n')
        {
            UString s = encoding_converter::gbkToUnicode(buff);
            buff.clear();

            SimpleDocLine &sl = scu::grow(m_lines);
            sl.setContent(std::move(s));
        }
    }

    // 把buff中的残留字节解析并添加进去
    if (!buff.empty())
    {
        SimpleDocLine & sl = scu::grow(m_lines);
        UString s = encoding_converter::gbkToUnicode(buff);
        buff.clear();
        sl.setContent(std::move(s));
    }

    // 如果最后一行以换行结束，则添加一个空行
    if (!m_lines.empty())
    {
        const SimpleDocLine & lastLine = m_lines.back();
        const CharN charCnt = lastLine.charCnt();
        if (charCnt > 0)
        {
            const UChar lastChar = lastLine.charAt(charCnt - 1);
            if (lastChar == UChar('\n') || lastChar == UChar('\r'))
            {
                scu::grow(m_lines);
            }
        }
    }
}
