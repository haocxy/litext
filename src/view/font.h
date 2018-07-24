#pragma once

#include <QFont>
#include <QFontMetrics>

#include "model/model_define.h"

namespace view
{

    struct FontInfo
    {
        std::string family;
        int size = 0;
        bool bold = false;
    };

    class Font
    {
    public:
        static const int kDefaultTabSize = 4;

    public:
        Font() :m_metrics(m_font) {}

        void setFont(const FontInfo &fi);

        std::string family() const { return m_font.family().toStdString(); }

        int size() const { return m_font.pointSize(); }

        bool bold() const { return m_font.bold(); }

        int height() const { return m_metrics.height(); }
        int ascent() const { return m_metrics.ascent(); }
        int descent() const { return m_metrics.descent(); }

        int charWidth(UChar c) const;

        bool isFixWidth() const { return m_metrics.width('i') == m_metrics.width('x'); }

    private:
        QFont m_font;
        QFontMetrics m_metrics;
    };

}