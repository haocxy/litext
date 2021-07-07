#include "text_area_config.h"


namespace gui
{

Pixel TextAreaConfig::charWidth(UChar c) const
{
    // 换行符不占用空间
    if (c == '\n' || c == '\r') {
        return Pixel(0);
    }

    const bool fixWidth = font_.isFixWidth();
    const int widthForFix = font_.charWidth('a');

    // tab符特殊处理
    if (c == '\t') {
        if (fixWidth) {
            // *[]*[]*[]*[]*
            return Pixel(hMargin_.value() * (tabSize_ - 1) + widthForFix * tabSize_);
        } else {
            return Pixel(hMargin_.value() * (tabSize_ - 1) + font_.charWidth(' ') * tabSize_);
        }
    }

    // 不是等宽字体则直接返回宽度
    if (!fixWidth) {
        return Pixel(font_.charWidth(c));
    }

    // 下面处理等宽字体

    const int rawWidth = font_.charWidth(c);
    if (rawWidth > widthForFix) {
        // 如果当前字符宽度大于单字符宽度，则固定占用两个字符
        return Pixel(hMargin_.value() + widthForFix * 2);
    } else {
        // 当前字符是普通的等宽单字符
        return Pixel(widthForFix);
    }
}

}
