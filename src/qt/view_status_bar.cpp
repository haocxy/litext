#include "status_bar_widget.h"

#include <cassert>

#include <QPainter>

#include "view/view.h"
#include "config/view_config.h"
#include "qt_util.h"


namespace
{

enum
{
kMargin = 5,
kDoubleMargin = kMargin << 1,
};

}

StatusBarWidget::StatusBarWidget(View * view, QWidget * parent)
	: QWidget(parent)
	, m_view(*view)
{
	assert(view);

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

QSize StatusBarWidget::sizeHint() const
{
	return QSize(-1, m_view.config().statusBarFont().height() + kDoubleMargin);
}

void StatusBarWidget::paintEvent(QPaintEvent * e)
{
	const view::Font & font = m_view.config().font();
	const view::Font & barFont = m_view.config().statusBarFont();

	QFont qfont;
	QtUtil::fillQFont(barFont, qfont);

	QPainter p(this);
	p.setPen(Qt::blue);
	p.setBrush(Qt::blue);
	p.drawRect(rect());
	p.setPen(Qt::white);
	p.setFont(qfont);
	p.drawText(kDoubleMargin, kMargin + barFont.ascent(),
		QString("%1 (%2)").arg(font.family().c_str()).arg(font.size()));
}


