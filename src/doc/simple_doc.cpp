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
                SimpleRow & row = scu::grow(m_lines);
                UString s = encoding_converter::gbkToUnicode(buff);
                buff.clear();
                row.setContent(std::move(s));
                row.setRowEnd(RowEnd::LF);
            }
            else
            {
                buff.push_back(c);
            }
            break;
        case ST_CR:
            if (c == '\r')
            {
                SimpleRow & row = scu::grow(m_lines);
                UString s = encoding_converter::gbkToUnicode(buff);
                buff.clear();
                row.setContent(std::move(s));
                row.setRowEnd(RowEnd::CR);
            }
            else if (c == '\n')
            {
                SimpleRow & row = scu::grow(m_lines);
                UString s = encoding_converter::gbkToUnicode(buff);
                buff.clear();
                row.setContent(std::move(s));
                row.setRowEnd(RowEnd::CRLF);

                state = ST_Normal;
            }
            else
            {
                SimpleRow & row = scu::grow(m_lines);
                UString s = encoding_converter::gbkToUnicode(buff);
                buff.clear();
                row.setContent(std::move(s));
                row.setRowEnd(RowEnd::CR);

                state = ST_Normal;
            }
            break;
        default:
            break;
        }
    } // while end

    if (state == ST_CR)
    {
        SimpleRow & row = scu::grow(m_lines);
        UString s = encoding_converter::gbkToUnicode(buff);
        buff.clear();
        row.setContent(std::move(s));
        row.setRowEnd(RowEnd::CR);
    }
    else
    {
        // ��buff�еĲ����ֽڽ�������ӽ�ȥ
        if (!buff.empty())
        {
            SimpleRow & row = scu::grow(m_lines);
            UString s = encoding_converter::gbkToUnicode(buff);
            buff.clear();
            row.setContent(std::move(s));
        }
    }

    // ������һ���Ի��н����������һ������
    if (m_lines.empty())
    {
        SimpleRow & row = scu::grow(m_lines);
        row.setRowEnd(RowEnd::NO);
    }
    else
    {
        const SimpleRow & lastRow = m_lines.back();

        if (lastRow.rowEnd() != RowEnd::NO)
        {
            SimpleRow & row = scu::grow(m_lines);
            row.setRowEnd(RowEnd::NO);
        }
    }
}
