#include "simple_doc.h"

#include <fstream>

#include "util/stl_container_util.h"

#include "text/encoding_converter.h"

void SimpleDoc::LoadFromFile(const std::string &path)
{
    namespace scu = StlContainerUtil;

    enum State { ST_Normal, ST_CR };

    int state = ST_Normal;

    std::ifstream ifs(path);

    std::string buff;

    char c = 0;

    while (ifs.read(&c, sizeof(c)))
    {
        switch (state)
        {
        case ST_Normal:
            if (c == '\r')
            {
                state = ST_CR;
            }
            else if (c == '\n')
            {
                SimpleDocLine & sl = scu::grow(m_lines);
                UString s = encoding_converter::gbkToUnicode(buff);
                buff.clear();
                sl.setContent(std::move(s));
                sl.setLineEnd(LineEnd::LF);
            }
            else
            {
                buff.push_back(c);
            }
            break;
        case ST_CR:
            if (c == '\r')
            {
                SimpleDocLine & sl = scu::grow(m_lines);
                UString s = encoding_converter::gbkToUnicode(buff);
                buff.clear();
                sl.setContent(std::move(s));
                sl.setLineEnd(LineEnd::CR);
            }
            else if (c == '\n')
            {
                SimpleDocLine & sl = scu::grow(m_lines);
                UString s = encoding_converter::gbkToUnicode(buff);
                buff.clear();
                sl.setContent(std::move(s));
                sl.setLineEnd(LineEnd::CRLF);

                state = ST_Normal;
            }
            else
            {
                SimpleDocLine & sl = scu::grow(m_lines);
                UString s = encoding_converter::gbkToUnicode(buff);
                buff.clear();
                sl.setContent(std::move(s));
                sl.setLineEnd(LineEnd::CR);

                state = ST_Normal;
            }
            break;
        default:
            break;
        }
    } // while end

    if (state == ST_CR)
    {
        SimpleDocLine & sl = scu::grow(m_lines);
        UString s = encoding_converter::gbkToUnicode(buff);
        buff.clear();
        sl.setContent(std::move(s));
        sl.setLineEnd(LineEnd::CR);
    }
    else
    {
        // 把buff中的残留字节解析并添加进去
        if (!buff.empty())
        {
            SimpleDocLine & sl = scu::grow(m_lines);
            UString s = encoding_converter::gbkToUnicode(buff);
            buff.clear();
            sl.setContent(std::move(s));
        }
    }

    // 如果最后一行以换行结束，则添加一个空行
    if (m_lines.empty())
    {
        SimpleDocLine & b = scu::grow(m_lines);
        b.setLineEnd(LineEnd::NO);
    }
    else
    {
        const SimpleDocLine & lastLine = m_lines.back();

        if (lastLine.lineEnd() != LineEnd::NO)
        {
            SimpleDocLine & b = scu::grow(m_lines);
            b.setLineEnd(LineEnd::NO);
        }
    }
}
