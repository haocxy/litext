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


StatusBarWidget::StatusBarWidget(TextArea &textArea)
	: textArea_(textArea)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    doc::Document &doc = textArea_.editor().document();

    updateContent();

    connect(this, &StatusBarWidget::qtSigCharsetDetected, this, &StatusBarWidget::qtSlotCharsetDetect);
    connect(this, &StatusBarWidget::qtSigUpdateStatus, this, &StatusBarWidget::qtSlotUpdateStatus);

    sigConns_ += doc.sigCharsetDetected().connect([this](Charset charset) {
        emit qtSigCharsetDetected(QString::fromUtf8(CharsetUtil::charsetToStr(charset)));
    });

    sigConns_ += doc.sigPartLoaded().connect([this](const doc::PartLoadedEvent &progress) {
        const int percent = progress.fileSize() == 0 ? 100 : (progress.partOffset() + progress.partSize()) * 100 / progress.fileSize();
        emit qtSigUpdateStatus(QString::asprintf("Loading: %2d%%", percent));
    });

    sigConns_ += doc.sigAllLoaded().connect([this]{
        emit qtSigUpdateStatus("All Loaded");
    });
}

QSize StatusBarWidget::sizeHint() const
{
	return QSize(-1, fontMetrics().height() + kDoubleMargin);
}

void StatusBarWidget::paintEvent(QPaintEvent *e)
{
	QPainter p(this);
    p.drawLine(0, 0, width(), 0);
	p.drawText(kDoubleMargin, kMargin + fontMetrics().ascent(), content_);
}

static const QString partNameCharset = "Charset: ";

static const QString sep = "  |  ";

void StatusBarWidget::updateContent()
{
    content_.clear();
    if (!status_.isEmpty()) {
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

void StatusBarWidget::qtSlotUpdateStatus(const QString &status)
{
    status_ = status;
    updateContent();
}


}
