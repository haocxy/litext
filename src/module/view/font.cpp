#include "font.h"

int Font::width(UChar c)
{
    // ���з���ռ�ÿռ�
    if (c == UChar('\n') || c == UChar('\r'))
    {
        return 0;
    }

    const bool fixWidth = isFixWidth();
    const int widthForFix = m_metrics.width('a');

    // tab�����⴦��
    if (c == UChar('\t'))
    {
        if (fixWidth)
        {
            // *[]*[]*[]*[]*
            return m_margin * (m_tabSize - 1) + widthForFix * m_tabSize;
        }
        else
        {
            return m_margin * (m_tabSize - 1) + m_metrics.width(' ') * m_tabSize;
        }
    }

    // ���ǵȿ�������ֱ�ӷ��ؿ��
    if (!fixWidth)
    {
        return m_metrics.width(c);
    }

    // ���洦��ȿ�����

    const int rawWidth = m_metrics.width(c);
    if (rawWidth > widthForFix)
    {
        // �����ǰ�ַ���ȴ��ڵ��ַ���ȣ���̶�ռ�������ַ�
        return m_margin + widthForFix * 2;
    }
    else
    {
        // ��ǰ�ַ�����ͨ�ĵȿ��ַ�
        return widthForFix;
    }
}
