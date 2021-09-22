#include "text_area_widget.h"

#include <QPainter>
#include <QMouseEvent>

#include "gui/text_area.h"
#include "editor/editor.h"


namespace gui::qt
{

TextAreaWidget::TextAreaWidget(TextArea &textArea)
    : textArea_(textArea)
{
    setCursor(Qt::IBeamCursor);
    setAttribute(Qt::WA_InputMethodEnabled);
    setFocusPolicy(Qt::ClickFocus);

    setMinimumWidth(500);
    setMinimumHeight(400);

    connect(this, &TextAreaWidget::qtSigShouldRepaint, this, &TextAreaWidget::qtSlotShouldRepaint);
    sigConns_ += textArea_.sigShouldRepaint().connect([this] {
        emit qtSigShouldRepaint();
    });
    sigConns_ += textArea_.sigViewportChanged().connect([this] {
        emit qtSigShouldRepaint();
    });
}

TextAreaWidget::~TextAreaWidget()
{

}

void TextAreaWidget::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.drawImage(0, 0, textArea_.widgetImg());
}

void TextAreaWidget::resizeEvent(QResizeEvent *e)
{
    const QSize sz(size());

    textArea_.resize({ sz.width(), sz.height() });

    update();
}

void TextAreaWidget::keyPressEvent(QKeyEvent *e)
{
    const int key = e->key();

    switch (key) {
    case Qt::Key_Up:
        textArea_.moveCursor(TextArea::Dir::Up);
        update();
        break;
    case Qt::Key_Down:
        textArea_.moveCursor(TextArea::Dir::Down);
        update();
        break;
    case Qt::Key_Left:
        textArea_.moveCursor(TextArea::Dir::Left);
        update();
        break;
    case Qt::Key_Right:
        textArea_.moveCursor(TextArea::Dir::Right);
        update();
        break;
    case Qt::Key_PageUp:
        textArea_.movePage(TextArea::Dir::Up);
        break;
    case Qt::Key_PageDown:
        textArea_.movePage(TextArea::Dir::Down);
        break;
    default:
        break;
    }
}

void TextAreaWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        textArea_.putCursor(e->x(), e->y());
        update();
    }
}

RowN TextAreaWidget::rowOffset() const
{
    return textArea_.rowOff();
}

void TextAreaWidget::qtSlotShouldRepaint()
{
    update();
}

}
