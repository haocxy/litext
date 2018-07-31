#include "line_num_bar.h"

#include <QPainter>

LineNumBar::LineNumBar(View * view, QWidget * parent)
    : QWidget(parent)
    , m_view(*view)
{
    QSizePolicy sizePolicy;
    sizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
    sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);

    setFixedWidth(50);
    setSizePolicy(sizePolicy);
}

LineNumBar::~LineNumBar()
{
}

void LineNumBar::paintEvent(QPaintEvent * e)
{
    QPainter p(this);
    p.fillRect(rect(), QColor(Qt::blue).lighter(180));
}


