#include "ruler_widget.h"

#include <QPainter>

#include "gui/text_area.h"
#include "gui/text_area_config.h"
#include "font_to_qfont.h"


namespace
{
    const QColor kBgColor = QColor(Qt::blue).lighter(190);
    const QColor kNormalColor = QColor(Qt::gray);
    const QColor kLastActColor = QColor(Qt::black);
}


namespace gui::qt
{


RulerWidget::RulerWidget(TextArea &textArea)
    : textArea_(textArea)
{
    QSizePolicy sizePolicy;
    sizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
    sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);

    setFixedWidth(50);
    setSizePolicy(sizePolicy);

    connect(this, &RulerWidget::qtSigShouldRepaint, [this] {
        update();
    });
    textAreaSigConns_ += textArea_.sigViewLocChanged().connect([this] {
        emit qtSigShouldRepaint();
    });
    textAreaSigConns_ += textArea_.sigShouldRepaint().connect([this] {
        emit qtSigShouldRepaint();
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
    const QFont font = fontToQFont(textArea_.config().font());
    p.setFont(font);
    p.setPen(kNormalColor);

    const int32_t lineNumOffset = textArea_.config().lineNumOffset();

    textArea_.drawEachLineNum([&p, lineNumOffset](RowN lineNum, i32 baseline, const RowBound &bound, bool isLastAct) {
        const QString s = QString::number(lineNum + lineNumOffset);

        if (isLastAct) {
            p.setPen(kLastActColor);
        }

        p.drawText(10, baseline, s);

        if (isLastAct) {
            p.setPen(kNormalColor);
        }
    });
}


}

