#include "ruler_widget.h"

#include <QPainter>

#include "vui/text_area_view.h"
#include "vui/font_to_qfont.h"
#include "config/view_config.h"


namespace
{
    const QColor kBgColor = QColor(Qt::blue).lighter(190);
    const QColor kNormalColor = QColor(Qt::gray);
    const QColor kLastActColor = QColor(Qt::black);
}

RulerWidget::RulerWidget(TextAreaView *view, QWidget *parent)
    : QWidget(parent)
    , m_view(*view)
{
    QSizePolicy sizePolicy;
    sizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
    sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);

    setFixedWidth(50);
    setSizePolicy(sizePolicy);

    m_cbhUpdate = m_view.addOnUpdateListener([this] {
        update();
    });

	m_cbhViewLocChange = m_view.addOnViewLocChangeListener([this] {
		update();
	});
}

RulerWidget::~RulerWidget()
{

}

void RulerWidget::paintEvent(QPaintEvent * e)
{
    QPainter p(this);
    paintBackground(p);
    paintLineNum(p);
}

void RulerWidget::paintBackground(QPainter & p)
{
    p.fillRect(rect(), kBgColor);
}

void RulerWidget::paintLineNum(QPainter & p)
{
    const QFont font = view::fontToQFont(m_view.config().font());
    p.setFont(font);
    p.setPen(kNormalColor);

    const int32_t lineNumOffset = m_view.config().lineNumOffset();

    m_view.drawEachLineNum([&p, lineNumOffset, this](RowN lineNum, int baseline, const view::RowBound & bound, bool isLastAct) {
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


