#include "font.h"


static bool checkIsFixWidth(const QFontMetrics &m) {
    // TODO 这种实现并不好，更好的实现是维护一组常用的已知的等宽字体的名字
    return m.horizontalAdvance('i') == m.horizontalAdvance('x');
}

void Font::setFont(const FontInfo &fi)
{
    font_.setFamily(fi.family.c_str());
    font_.setPointSize(fi.size);
    font_.setBold(fi.bold);

    metrics_ = QFontMetrics(font_);

    isFixWidth_ = checkIsFixWidth(metrics_);
}
