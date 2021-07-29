#pragma once

#include <map>
#include <unordered_map>

#include "core/font_index.h"
#include "core/font.h"

#include "glyph_width_provider.h"


namespace doc
{

class GlyphWidthCache : public GlyphWidthProvider {
public:
    GlyphWidthCache(const font::FontIndex &fontIndex, int pointSize);

    virtual ~GlyphWidthCache() {}

    virtual int glyphWidth(char32_t unicode) override;

private:
    font::FontContext context_;
    font::FontFile fontFile_;
    font::FontFace fontFace_;
    std::unordered_map<char32_t, int> unicodeToWidth_{ 65536 };
};

}
