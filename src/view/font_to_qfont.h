#pragma once

#include "font.h"
#include <QFont>

namespace view
{
    void fontToQFont(const view::Font & font, QFont & qfont)
    {
        qfont.setFamily(font.family().c_str());
        qfont.setPointSize(font.size());
        qfont.setBold(font.bold());
    }

    QFont fontToQFont(const view::Font & font)
    {
        QFont qfont;
        fontToQFont(font, qfont);
        return qfont;
    }
}
