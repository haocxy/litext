#pragma once

namespace gui
{

class CharPixWidthProvider {
public:
    virtual ~CharPixWidthProvider() {}

    // 看起来是获取数据，实际上可能修改缓存，所以不能是 const
    virtual int charWidth(char32_t unicode) = 0;
};

}