#include "status_bar_widget.h"

#include <cassert>

#include <QPainter>

#include "doc/async_doc_server.h"
#include "editor/editor.h"
#include "gui/text_area.h"
#include "gui/text_area_config.h"


namespace
{

enum {
    kMargin = 8,
    kDoubleMargin = kMargin << 1,
};

}


namespace gui::qt
{


StatusBarWidget::StatusBarWidget(TextArea &view, QWidget *parent)
	: QWidget(parent)
	, view_(view)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	doc::AsyncDocServer &server = view.editor().server();
	cbhCharsetDetected_ = server.addCharsetDetectedCallback([this](doc::Charset charset) {
		setContent(tr("charset detected: %1").arg(QString::fromStdString(doc::CharsetUtil::charsetToStr(charset))));
	});
}

QSize StatusBarWidget::sizeHint() const
{
	return QSize(-1, fontMetrics().height() + kDoubleMargin);
}

void StatusBarWidget::paintEvent(QPaintEvent *e)
{
	QPainter p(this);
	p.setPen(Qt::blue);
	p.setBrush(Qt::blue);
	p.drawRect(rect());
	p.setPen(Qt::white);

	p.drawText(kDoubleMargin, kMargin + fontMetrics().ascent(), content_);
}

void StatusBarWidget::setContent(QString &&content)
{
	content_ = std::move(content);

	update();
}


}
