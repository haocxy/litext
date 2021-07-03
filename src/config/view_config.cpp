#include "view_config.h"

int view::Config::charWidth(QChar c) const
{
    // 换行符不占用空间
    if (c == UChar('\n') || c == UChar('\r'))
    {
        return 0;
    }

    const bool fixWidth = m_font.isFixWidth();
    const int widthForFix = m_font.charWidth('a');

    // tab符特殊处理
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

    // 不是等宽字体则直接返回宽度
    if (!fixWidth)
    {
        return m_font.charWidth(c);
    }

    // 下面处理等宽字体

    const int rawWidth = m_font.charWidth(c);
    if (rawWidth > widthForFix)
    {
        // 如果当前字符宽度大于单字符宽度，则固定占用两个字符
        return m_hMargin + widthForFix * 2;
    }
    else
    {
        // 当前字符是普通的等宽单字符
        return widthForFix;
    }
}
