#include "font_info.h"

#include <cwctype>

int FontInfoProvider::GetCharWidth(wchar_t c) const
{
    const int margin = m_drawConfig.GetCharMargin();
    const int tabSize = m_drawConfig.GetTabSize();

    if (c == '\n' || c == '\r')
    {
        return 0;
    }

    if (c == '\t')
    {
        if (m_isFixWide)
        {
            // *[]*[]*[]*[]*
            return margin * (tabSize - 1) + m_charWidthForFixWide * tabSize;
        }
        else
        {
            return margin * (tabSize - 1) + m_metrics.width(' ') * tabSize;
        }
    }

    if (m_isFixWide)
    {
        const int rawWidth = m_metrics.width(c);
        if (rawWidth > m_charWidthForFixWide)
        {
            return margin + m_charWidthForFixWide * 2;
        }
        else
        {
            return m_charWidthForFixWide;
        }
    }

    return m_metrics.width(c);
}
