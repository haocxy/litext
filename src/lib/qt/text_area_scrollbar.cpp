#include "text_area_scrollbar.h"

#include <QStyle>
#include <QStyleOptionSlider>
#include <QMouseEvent>


namespace gui::qt
{

TextAreaScrollBar::TextAreaScrollBar(QWidget *parent)
    : QScrollBar(parent)
{
    setContextMenuPolicy(Qt::NoContextMenu);
    setOrientation(Qt::Vertical);
    setRange(0, 0);
    connect(this, &TextAreaScrollBar::valueChanged, this, &TextAreaScrollBar::jumpValueChanged);
}

TextAreaScrollBar::~TextAreaScrollBar()
{
}

void TextAreaScrollBar::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton || e->button() == Qt::RightButton) {
        jumping_ = true;
        jumpToY(e->y());
    }
}

void TextAreaScrollBar::mouseReleaseEvent(QMouseEvent *e)
{
    if (!jumping_) {
        return;
    }

    if (e->button() == Qt::LeftButton || e->button() == Qt::RightButton) {
        jumping_ = false;
    }
}

void TextAreaScrollBar::mouseMoveEvent(QMouseEvent *e)
{
    if (jumping_) {
        jumpToY(e->y());
    }
}

void TextAreaScrollBar::jumpToY(int y)
{
    QStyleOptionSlider opt;
    initStyleOption(&opt);

    const QStyle *s = style();
    const QRect recSlider = s->subControlRect(QStyle::CC_ScrollBar, &opt, QStyle::SC_ScrollBarSlider, this);
    const QRect recGroove = s->subControlRect(QStyle::CC_ScrollBar, &opt, QStyle::SC_ScrollBarGroove, this);

    const int min = minimum();
    const int max = maximum();
    const int pos = y - recGroove.y();
    const int space = recGroove.bottom() - recGroove.y() - recSlider.height() + 1;
    const bool upsideDown = opt.upsideDown;

    const int value = QStyle::sliderValueFromPosition(min, max, pos, space, upsideDown);

    setValue(value);
}

}
