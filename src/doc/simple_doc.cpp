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

    // ��buff�еĲ����ֽڽ�������ӽ�ȥ
    if (!buff.empty())
    {
        SimpleDocLine & sl = scu::grow(m_lines);
        UString s = encoding_converter::gbkToUnicode(buff);
        buff.clear();
        sl.setContent(std::move(s));
    }

    // ������һ���Ի��н����������һ������
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
