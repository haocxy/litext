#include "text_area_config.h"


namespace gui
{

void TextAreaConfig::setFont(const FontOld &font)
{
    font_ = font;
    isFixWidth_ = font_.isFixWidth();
    widthForFix_ = font_.charWidth('a');
}

Pixel::Raw TextAreaConfig::charWidth(UChar c) const
{
    // 换行符不占用空间
    if (c == '\n' || c == '\r') {
        return 0;
    }

    // tab符特殊处理
    if (c == '\t') {
        if (isFixWidth_) {
            // *[]*[]*[]*[]*
            return hMargin_ * (tabSize_ - 1) + widthForFix_ * tabSize_;
        } else {
            return hMargin_ * (tabSize_ - 1) + font_.charWidth(' ') * tabSize_;
        }
    }

    // 不是等宽字体则直接返回宽度
    if (!isFixWidth_) {
        return font_.charWidth(c);
    }

    // 下面处理等宽字体

    const int rawWidth = font_.charWidth(c);
    if (rawWidth > widthForFix_) {
        // 如果当前字符宽度大于单字符宽度，则固定占用两个字符
        return hMargin_ + widthForFix_ * 2;
    } else {
        // 当前字符是普通的等宽单字符
        return widthForFix_;
    }
}

}
