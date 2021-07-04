#include "status_bar_widget.h"

#include <cassert>

#include <QPainter>

#include "gui/text_area.h"
#include "gui/text_area_config.h"
#include "qt_util.h"


namespace
{

enum {
    kMargin = 5,
    kDoubleMargin = kMargin << 1,
};

}

StatusBarWidget::StatusBarWidget(TextArea *view, QWidget * parent)
	: QWidget(parent)
	, view_(*view)
{
	assert(view);

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

QSize StatusBarWidget::sizeHint() const
{
	return QSize(-1, view_.config().statusBarFont().height() + kDoubleMargin);
}

void StatusBarWidget::paintEvent(QPaintEvent * e)
{
	const view::Font &font = view_.config().font();
	const view::Font &barFont = view_.config().statusBarFont();

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


