#include "status_bar_widget.h"

#include <cassert>
#include <algorithm>

#include <QPainter>

#include "core/logger.h"
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

static i64 loadPercent(const doc::LoadProgress &p) {
    if (p.done()) {
        return 100;
    }

    const i64 total = p.totalByteCount();
    if (total <= 0) {
        return 100;
    }

    return p.loadedByteCount() * 100 / total;
}

StatusBarWidget::StatusBarWidget(TextArea &textArea)
	: textArea_(textArea)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    doc::Document &doc = textArea_.editor().document();

    updateContent();

    connect(this, &StatusBarWidget::qtSigCharsetDetected, this, &StatusBarWidget::qtSlotCharsetDetect);
    connect(this, &StatusBarWidget::qtSigUpdateStatus, this, &StatusBarWidget::qtSlotUpdateStatus);
    connect(this, &StatusBarWidget::qtSigRowCountUpdated, this, &StatusBarWidget::qtSlotRowCountUpdated);

    sigConns_ += doc.sigCharsetDetected().connect([this](Charset charset) {
        emit qtSigCharsetDetected(QString::fromUtf8(CharsetUtil::charsetToStr(charset)));
    });

    sigConns_ += doc.sigLoadProgress().connect([this](const doc::LoadProgress &p) {
        emit qtSigUpdateStatus(QString::asprintf("Loading: %2d%%", loadPercent(p)));
    });

    sigConns_ += doc.sigAllLoaded().connect([this]{
        emit qtSigUpdateStatus("All Loaded");
    });

    sigConns_ += doc.sigRowCountUpdated().connect([this](RowN rowCount) {
        emit qtSigRowCountUpdated(QString("RowCount: %1").arg(rowCount));
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
    if (!rowCount_.isEmpty()) {
        content_.append(sep);
        content_.append(rowCount_);
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

void StatusBarWidget::qtSlotRowCountUpdated(const QString &rowCount)
{
    rowCount_ = rowCount;
    updateContent();
}


}
