#pragma once

#include <map>
#include <unordered_map>

#include "core/font_index.h"
#include "core/font.h"

#include "char_pix_width_provider.h"


namespace gui
{

class CachedCharPixWidthProvider : public CharPixWidthProvider {
public:
    CachedCharPixWidthProvider(const font::FontIndex &fontIndex, int pointSize);

    virtual ~CachedCharPixWidthProvider() {}

    virtual int charWidth(char32_t unicode) override;

private:
    font::FontContext context_;
    font::FontFile fontFile_;
    font::FontFace fontFace_;
    std::unordered_map<char32_t, int> cacheUnicodeToPixWidth_{ 65536 };
};

}
