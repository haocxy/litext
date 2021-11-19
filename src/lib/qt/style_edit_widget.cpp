#include "style_edit_widget.h"

#include <QKeyEvent>


namespace gui::qt
{

StyleEditWidget::StyleEditWidget(QWidget *parent)
    : QTextEdit(parent) {

    initFont();
}

StyleEditWidget::~StyleEditWidget()
{

}

QString StyleEditWidget::content() const
{
    return toPlainText();
}

void StyleEditWidget::keyPressEvent(QKeyEvent *e)
{
    if (e->modifiers().testFlag(Qt::KeyboardModifier::ControlModifier)) {
        switch (e->key()) {
        case Qt::Key::Key_Return:
            emit shouldApply();
            return;
        default:
            break;
        }
    }

    QTextEdit::keyPressEvent(e);
}

void StyleEditWidget::initFont()
{
    setFont(QFont("Consolas", 14));
}

}
