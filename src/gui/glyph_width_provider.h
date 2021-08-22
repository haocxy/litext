#pragma once


namespace gui
{

class GlyphWidthProvider {
public:
    virtual ~GlyphWidthProvider() {}

    // 看起来是获取数据，实际上可能修改缓存，所以不能是 const
    virtual int glyphWidth(char32_t unicode) = 0;
};

}
