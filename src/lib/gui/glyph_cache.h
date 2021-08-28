#pragma once

#include <unordered_map>

#include <QImage>

#include "core/font.h"
#include "core/font_index.h"


namespace gui
{

class GlyphCache {
public:
    GlyphCache() {}

    const font::FontFace &face() const {
        return face_;
    }

    void setFont(const font::FontIndex &index, i32 pointSize) {
        if (index.file() == file_.path() && index.faceIndex() == face_.faceIndex()) {
            return;
        }

        glyphs_.clear();

        // 必须先反向把face和file关闭
        // 不能直接重新设置新的对象，这会导致face处于悬空状态
        face_ = font::FontFace();
        file_ = font::FontFile();
        file_ = font::FontFile(context_, index.file());
        face_ = font::FontFace(file_, index.faceIndex());
        face_.setDpi(SystemUtil::screenHorizontalDpi(), SystemUtil::screenVerticalDpi());
        face_.setPointSize(pointSize);
    }

    const font::Glyph &glyphOf(char32_t unicode) const {
        return ensureCached(unicode);
    }

    i32 advance(char32_t unicode) const {
        return ensureCached(unicode).advance();
    }

private:
    const font::Glyph &ensureCached(char32_t unicode) const {
        font::Glyph &g = glyphs_[unicode];
        if (g) {
            return g;
        }

        const i64 glyphIndex = face_.mapUnicodeToGlyphIndex(unicode);
        face_.loadGlyph(glyphIndex);
        face_.renderGlyph();
        g.init(face_);

        return g;
    }

private:
    font::FontContext context_;
    font::FontFile file_;
    mutable font::FontFace face_;
    mutable std::unordered_map<char32_t, font::Glyph> glyphs_{ 65536 };
};

}
