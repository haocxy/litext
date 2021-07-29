#include "glyph_width_cache.h"


namespace doc
{



GlyphWidthCache::GlyphWidthCache(const font::FontIndex &fontIndex, int pointSize)
    : fontFile_(context_, fontIndex.file())
    , fontFace_(fontFile_, fontIndex.faceIndex())
{
    fontFace_.setPointSize(pointSize);
}

int GlyphWidthCache::glyphWidth(char32_t unicode)
{
    int &pixWidth = unicodeToWidth_[unicode];
    if (pixWidth != 0) {
        return pixWidth;
    }

    const int64_t glyphIndex = fontFace_.mapUnicodeToGlyphIndex(unicode);
    fontFace_.loadGlyph(glyphIndex);
    pixWidth = fontFace_.bitmap().width;
    return pixWidth;
}

}
