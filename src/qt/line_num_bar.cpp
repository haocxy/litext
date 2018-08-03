#include "line_num_bar.h"

#include <QPainter>

#include "view/view.h"
#include "view/font_to_qfont.h"
#include "view/view_config.h"

namespace
{
    const QColor kBgColor = QColor(Qt::blue).lighter(190);
    const QColor kNormalColor = QColor(Qt::gray);
    const QColor kLastActColor = QColor(Qt::black);
}

LineNumBar::LineNumBar(View * view, QWidget * parent)
    : QWidget(parent)
    , m_view(*view)
{
    QSizePolicy sizePolicy;
    sizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
    sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);

    setFixedWidth(50);
    setSizePolicy(sizePolicy);

    m_listenerIdForUpdate = m_view.addOnUpdateListener([this] {
        update();
    });
}

LineNumBar::~LineNumBar()
{
    m_view.removeOnUpdateListener(m_listenerIdForUpdate);
}

void LineNumBar::paintEvent(QPaintEvent * e)
{
    QPainter p(this);
    paintBackground(p);
    paintLineNum(p);
}

void LineNumBar::paintBackground(QPainter & p)
{
    p.fillRect(rect(), kBgColor);
}

void LineNumBar::paintLineNum(QPainter & p)
{
    const QFont font = view::fontToQFont(m_view.config().font());
    p.setFont(font);
    p.setPen(kNormalColor);

    const int32_t lineNumOffset = m_view.config().lineNumOffset();

    m_view.drawEachLineNum([&p, lineNumOffset, this](RowN lineNum, int baseline, const view::PhaseBound & bound, bool isLastAct) {
        const QString s = QString::number(lineNum + lineNumOffset);

        if (isLastAct)
        {
            p.setPen(kLastActColor);
        }

        p.drawText(10, baseline, s);

        if (isLastAct)
        {
            p.setPen(kNormalColor);
        }
    });
}


