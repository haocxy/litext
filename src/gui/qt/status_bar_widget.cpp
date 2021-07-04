#include "status_bar_widget.h"

#include <cassert>

#include <QPainter>

#include "gui/text_area.h"
#include "gui/text_area_config.h"
#include "qt_util.h"


namespace
{

enum {
    kMargin = 8,
    kDoubleMargin = kMargin << 1,
};

}


namespace gui::qt
{


StatusBarWidget::StatusBarWidget(gui::TextArea *view, QWidget *parent)
	: QWidget(parent)
	, view_(*view)
{
	assert(view);

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

QSize StatusBarWidget::sizeHint() const
{
	return QSize(-1, fontMetrics().height() + kDoubleMargin);
}

void StatusBarWidget::paintEvent(QPaintEvent *e)
{
	const Font &font = view_.config().font();

	QPainter p(this);
	p.setPen(Qt::blue);
	p.setBrush(Qt::blue);
	p.drawRect(rect());
	p.setPen(Qt::white);

	p.drawText(kDoubleMargin, kMargin + fontMetrics().ascent(),
		QString("%1 (%2)").arg(font.family().c_str()).arg(font.size()));
}


}
