#include "popup_widget.h"


namespace gui::qt
{

PopupWidget::PopupWidget(QWidget *parent)
    : QFrame(parent) {

    setWindowFlag(Qt::FramelessWindowHint);
    setWindowFlag(Qt::Popup);
}

PopupWidget::~PopupWidget()
{
}

void PopupWidget::locate(QWidget *base)
{
    if (!base) {
        return;
    }

    const QRect baseGeo = base->geometry();
    const QPoint basePos = base->mapToGlobal(QPoint(0, 0));
    const int baseMidX = basePos.x() + baseGeo.width() / 2;
    const int x = baseMidX - width() / 2;
    const int y = basePos.y() - height();

    move(x, y);
}

void PopupWidget::mousePressEvent(QMouseEvent *e)
{
    setAttribute(Qt::WA_NoMouseReplay);
    QFrame::mousePressEvent(e);
}

void PopupWidget::locateWithoutParent()
{
    // TODO
    move(0, 0);
}

void PopupWidget::locateWithParent(const QWidget &parent)
{

}

void PopupWidget::locateRelativeTo(QWidget *parent)
{
    if (parent) {
        locateWithParent(*parent);
    } else {
        locateWithoutParent();
    }
}

}
