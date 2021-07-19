#include "status_bar_widget.h"

#include <cassert>

#include <QPainter>

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


StatusBarWidget::StatusBarWidget(TextArea &textArea, QWidget *parent)
	: QWidget(parent)
	, textArea_(textArea)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    connect(this, &StatusBarWidget::qtSigAllLoaded, this, &StatusBarWidget::qtSlotAllLoaded);

    Editor &editor = textArea.editor();
    doc::Document &document = editor.document();

    slotAllLoaded_ = document.sigAllLoaded().connect([this]{
        emit qtSigAllLoaded();
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

void StatusBarWidget::updateContent(QString &&content)
{
	content_ = std::move(content);

	update();
}

void StatusBarWidget::qtSlotAllLoaded()
{
    updateContent(tr("all loaded"));
}


}
