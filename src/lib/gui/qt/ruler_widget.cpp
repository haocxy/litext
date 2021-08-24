#include "ruler_widget.h"

#include <QPainter>

#include "doc/document.h"
#include "editor/editor.h"
#include "gui/text_area.h"
#include "gui/text_area_config.h"
#include "font_to_qfont.h"


namespace
{
const QColor kBgColor = QColor(Qt::blue).lighter(190);
const QColor kNormalColor = QColor(Qt::gray);
const QColor kLastActColor = QColor(Qt::black);
const int Margin = 10;
}

namespace gui::qt
{

RulerWidget::RulerWidget(TextArea &textArea)
    : textArea_(textArea)
{
    QSizePolicy sizePolicy;
    sizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
    sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);

    setSizePolicy(sizePolicy);
    setFixedWidth(0);

    setFont(fontToQFont(textArea_.config().font()));

    connect(this, &RulerWidget::qtSigUpdateContent, this, &RulerWidget::qtSlotUpdateContent);
    connect(this, &RulerWidget::qtSigUpdateWidth, this, &RulerWidget::qtSlotUpdateWidth);

    textAreaSigConns_ += textArea_.sigViewLocChanged().connect([this] {
        emit qtSigUpdateContent();
    });
    textAreaSigConns_ += textArea_.sigShouldRepaint().connect([this] {
        emit qtSigUpdateContent();
    });
    textAreaSigConns_ += textArea_.editor().doc().sigRowCountUpdated().connect([this](RowN row) {
        emit qtSigUpdateWidth(QString::number(row));
    });
}

RulerWidget::~RulerWidget()
{

}

void RulerWidget::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    paintBackground(p);
    paintLineNum(p);
}

void RulerWidget::paintBackground(QPainter &p)
{
    p.fillRect(rect(), kBgColor);
}

void RulerWidget::paintLineNum(QPainter &p)
{
    p.setPen(kNormalColor);

    const int32_t lineNumOffset = textArea_.config().lineNumOffset();

    textArea_.drawEachLineNum([&p, lineNumOffset](RowN lineNum, i32 baseline, const RowBound &bound, bool isLastAct) {
        const QString s = QString::number(lineNum + lineNumOffset);

        if (isLastAct) {
            p.setPen(kLastActColor);
        }

        p.drawText(Margin, baseline, s);

        if (isLastAct) {
            p.setPen(kNormalColor);
        }
    });
}

void RulerWidget::qtSlotUpdateContent()
{
    update();
}

void RulerWidget::qtSlotUpdateWidth(QString placeholder)
{
    const int w = Margin * 2 + fontMetrics().horizontalAdvance('9') * placeholder.size();
    if (w > width()) {
        setFixedWidth(w);
        update();
    }
}

}
