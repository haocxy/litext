#include "view_config.h"

int view::Config::charWidth(QChar c) const
{
    // ���з���ռ�ÿռ�
    if (c == UChar('\n') || c == UChar('\r'))
    {
        return 0;
    }

    const bool fixWidth = m_font.isFixWidth();
    const int widthForFix = m_font.charWidth('a');

    // tab�����⴦��
    if (c == UChar('\t'))
    {
        if (fixWidth)
        {
            // *[]*[]*[]*[]*
            return m_hMargin * (m_tabSize - 1) + widthForFix * m_tabSize;
        }
        else
        {
            return m_hMargin * (m_tabSize - 1) + m_font.charWidth(' ') * m_tabSize;
        }
    }

    // ���ǵȿ�������ֱ�ӷ��ؿ��
    if (!fixWidth)
    {
        return m_font.charWidth(c);
    }

    // ���洦��ȿ�����

    const int rawWidth = m_font.charWidth(c);
    if (rawWidth > widthForFix)
    {
        // �����ǰ�ַ���ȴ��ڵ��ַ���ȣ���̶�ռ�������ַ�
        return m_hMargin + widthForFix * 2;
    }
    else
    {
        // ��ǰ�ַ�����ͨ�ĵȿ��ַ�
        return widthForFix;
    }
}
