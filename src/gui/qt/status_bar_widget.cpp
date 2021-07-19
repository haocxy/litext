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

    Editor &editor = textArea.editor();
    doc::Document &document = editor.document();

    updateContent();

    connect(this, &StatusBarWidget::qtSigCharsetDetected, &StatusBarWidget::qtSlotCharsetDetect);
    slotCharsetDetected_ = document.sigCharsetDetected().connect([this](Charset charset) {
        emit qtSigCharsetDetected(QString::fromUtf8(CharsetUtil::charsetToStr(charset)));
    });

    connect(this, &StatusBarWidget::qtSigAllLoaded, this, &StatusBarWidget::qtSlotAllLoaded);
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

static const QString partNameStatus = "Status: ";
static const QString partNameCharset = "Charset: ";

static const QString sep = "  |  ";

void StatusBarWidget::updateContent()
{
    content_.clear();
    if (!status_.isEmpty()) {
        content_.append(partNameStatus);
        content_.append(status_);
    }
    if (!charset_.isEmpty()) {
        content_.append(sep);
        content_.append(partNameCharset);
        content_.append(charset_);
    }
    if (!filesize_.isEmpty()) {
        content_.append(sep);
        content_.append(filesize_);
    }

	update();
}

void StatusBarWidget::qtSlotCharsetDetect(const QString &charset)
{
    charset_ = charset;
    updateContent();
}

void StatusBarWidget::qtSlotAllLoaded()
{
    status_ = "All loaded";
    updateContent();
}


}
