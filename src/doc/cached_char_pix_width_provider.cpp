#include "cached_char_pix_width_provider.h"


namespace doc
{



CachedCharPixWidthProvider::CachedCharPixWidthProvider(const font::FontIndex &fontIndex, int pointSize)
    : fontFile_(context_, fontIndex.file())
    , fontFace_(fontFile_, fontIndex.faceIndex())
{
    fontFace_.setPointSize(pointSize);
}

int CachedCharPixWidthProvider::charWidth(char32_t unicode)
{
    int &pixWidth = cacheUnicodeToPixWidth_[unicode];
    if (pixWidth != 0) {
        return pixWidth;
    }

    const int64_t glyphIndex = fontFace_.mapUnicodeToGlyphIndex(unicode);
    fontFace_.loadGlyph(glyphIndex);
    pixWidth = fontFace_.bitmap().width;
    return pixWidth;
}

}
