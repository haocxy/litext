#include "font.h"

int view::Font::charWidth(UChar c) const
{
    return m_metrics.horizontalAdvance(QChar(c));
}

void view::Font::setFont(const FontInfo & fi)
{
    m_font.setFamily(fi.family.c_str());
    m_font.setPointSize(fi.size);
    m_font.setBold(fi.bold);

    m_metrics = QFontMetrics(m_font);
}
