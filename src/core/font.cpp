#include "font.h"


int Font::charWidth(UChar unicode) const
{
    if (!UCharUtil::needSurrogate(unicode)) {
        return metrics_.horizontalAdvance(QChar(unicode));
    } else {
        QString pair;
        pair.push_back(QChar::highSurrogate(unicode));
        pair.push_back(QChar::lowSurrogate(unicode));
        return metrics_.horizontalAdvance(pair);
    }
}

bool Font::isFixWidth() const {
    // TODO ����ʵ�ֲ����ã����õ�ʵ����ά��һ�鳣�õ���֪�ĵȿ����������
    return metrics_.horizontalAdvance('i') == metrics_.horizontalAdvance('x');
}

void Font::setFont(const FontInfo &fi)
{
    font_.setFamily(fi.family.c_str());
    font_.setPointSize(fi.size);
    font_.setBold(fi.bold);

    metrics_ = QFontMetrics(font_);
}
