#include "font.h"


int Font::charWidth(QChar c) const
{
    return m_metrics.horizontalAdvance(QChar(c));
}

void Font::setFont(const FontInfo & fi)
{
    m_font.setFamily(fi.family.c_str());
    m_font.setPointSize(fi.size);
    m_font.setBold(fi.bold);

    m_metrics = QFontMetrics(m_font);
}
