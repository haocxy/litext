#include "text_area_scrollbar.h"

#include <QStyle>
#include <QStyleOptionSlider>
#include <QMouseEvent>


namespace gui::qt
{

TextAreaScrollBar::TextAreaScrollBar(QWidget *parent)
    : QScrollBar(parent)
{
    setOrientation(Qt::Vertical);
    setRange(0, 0);
    connect(this, &TextAreaScrollBar::valueChanged, this, &TextAreaScrollBar::jumpValueChanged);
}

TextAreaScrollBar::~TextAreaScrollBar()
{
}

void TextAreaScrollBar::mousePressEvent(QMouseEvent *e)
{
    assert(orientation == Qt::Vertical);

    // 第一步: 如果按下的是鼠标左键, 则使视图直接跳转到当前位置
    // 这是区别于 Qt 滚动条默认逻辑的, Qt 滚动条的默认逻辑是慢慢地滚动到这一位置
    if (e->button() == Qt::LeftButton) {
        QStyleOptionSlider opt;
        initStyleOption(&opt);

        const QStyle *s = style();
        const QRect recSlider = s->subControlRect(QStyle::CC_ScrollBar, &opt, QStyle::SC_ScrollBarSlider, this);
        const QRect recGroove = s->subControlRect(QStyle::CC_ScrollBar, &opt, QStyle::SC_ScrollBarGroove, this);

        const int min = minimum();
        const int max = maximum();
        const int pos = e->y() - recGroove.y();
        const int space = recGroove.bottom() - recGroove.y() - recSlider.height() + 1;
        const bool upsideDown = opt.upsideDown;

        const int value = QStyle::sliderValueFromPosition(min, max, pos, space, upsideDown);

        setValue(value);
    }

    // 第二步: 无论按下的是否是鼠标左键, 都触发 Qt 滚动条的默认逻辑
    // 这样可以实现这种效果:
    //   当用户在滚动条滑块之外按下左键后, 文档会直接跳转到这个位置(而非默认的慢慢滚动)
    // 紧接着, 如果不松开鼠标左键, 则可以利用 Qt 的默认逻辑继续从当前位置跟着鼠标的滚动而滚动
    QScrollBar::mousePressEvent(e);
}

}
