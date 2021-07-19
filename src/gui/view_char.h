#pragma once

#include "core/uchar.h"

#include "pixel.h"


namespace gui
{

class VChar {
public:
    VChar() {}

    VChar(UChar unicode, Pixel::Raw x, Pixel::Raw width)
        : m_unicode(unicode), m_x(x), m_width(width) {}

    UChar uchar() const { return m_unicode; }
    Pixel::Raw x() const { return m_x; }

    void setUChar(UChar unicode) { m_unicode = unicode; }
    void setX(Pixel::Raw x) { m_x = x; }

    Pixel::Raw width() const { return m_width; }
    void setWidth(Pixel::Raw width) { m_width = width; }

private:
    UChar m_unicode = 0;
    Pixel::Raw m_x = 0;
    Pixel::Raw m_width = 0;
};

}
