#pragma once

#include <QFont>
#include <QFontMetrics>

#include "module/model/model_define.h"

class Font
{
public:
    static const int kDefaultMargin = 0;
    static const int kDefaultTabSize = 4;

public:
    Font() :m_metrics(m_font) {}

    std::string family() const { return m_font.family().toStdString(); }
    void setFamily(const std::string &family) { m_font.setFamily(family.c_str()); }

    int size() const { return m_font.pointSize(); }
    void setSize(int size) { m_font.setPointSize(size); }

    bool bold() const { return m_font.bold(); }
    void setBold(bool bold) { m_font.setBold(bold); }

    int margin() const { return m_margin; }
    void setMargin(int margin) { m_margin = margin; }

    int tabSize() const { return m_tabSize; }
    void setTabSize(int tabSize) { m_tabSize = tabSize; }

    int height() const { return m_metrics.height(); }
    int ascent() const { return m_metrics.ascent(); }
    int descent() const { return m_metrics.descent(); }

    int width(UChar c);

private:
    bool isFixWidth() const { return m_metrics.width('i') == m_metrics.width('x'); }

private:
    QFont m_font;
    QFontMetrics m_metrics;
    int m_margin = 0;
    int m_tabSize = kDefaultTabSize;
};
