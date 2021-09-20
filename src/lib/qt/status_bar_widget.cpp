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

StatusBarWidget::StatusBarWidget(TextArea &textArea)
	: textArea_(textArea)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    doc::Document &doc = textArea_.editor().document();

    updateContent();

    connect(this, &StatusBarWidget::qtSigFileSizeDetected, this, &StatusBarWidget::qtSlotFileSizeDetected);
    connect(this, &StatusBarWidget::qtSigLoadProgress, this, &StatusBarWidget::qtSlotLoadProgress);
    connect(this, &StatusBarWidget::qtSigCharsetDetected, this, &StatusBarWidget::qtSlotCharsetDetect);
    connect(this, &StatusBarWidget::qtSigUpdateStatus, this, &StatusBarWidget::qtSlotUpdateStatus);
    connect(this, &StatusBarWidget::qtSigRowCountUpdated, this, &StatusBarWidget::qtSlotRowCountUpdated);

    sigConns_ += doc.sigFileSizeDetected().connect([this](i64 fileSize) {
        emit qtSigFileSizeDetected(fileSize);
    });

    sigConns_ += doc.sigCharsetDetected().connect([this](Charset charset) {
        emit qtSigCharsetDetected(QString::fromUtf8(CharsetUtil::charsetToStr(charset)));
    });

    sigConns_ += doc.sigLoadProgress().connect([this](const doc::LoadProgress &p) {
        emit qtSigLoadProgress(p.loadedByteCount(), p.done());
    });

    sigConns_ += doc.sigAllLoaded().connect([this]{
        emit qtSigUpdateStatus(QString("Loaded by %1 ms").arg(textArea_.doc().loadTimeUsageMs()));
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
    if (errorMsg_.isEmpty()) {
        p.drawText(kDoubleMargin, kMargin + fontMetrics().ascent(), content_);
    } else {
        p.save();
        p.fillRect(rect(), qRgb(205, 51, 51));
        p.setPen(QPen(Qt::white));
        p.drawText(kDoubleMargin, kMargin + fontMetrics().ascent(), errorMsg_);
        p.restore();
    }
}

static const QString partNameCharset = "Charset: ";

static const QString sep = "  |  ";

void StatusBarWidget::showErrorMsg(const QString &msg)
{
    errorMsg_.clear();
    errorMsg_.append("OPEN FAILED");
    errorMsg_.append(sep);
    errorMsg_.append(msg);

    update();
}

void StatusBarWidget::updateContent()
{
    errorMsg_.clear();

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

void StatusBarWidget::qtSlotFileSizeDetected(long long fileSize)
{
    fileSize_ = fileSize;
}

static int loadPercent(long long loadedBytes, long long fileSize, bool done)
{
    if (fileSize > 0) {
        if (!done && loadedBytes < fileSize) {
            return static_cast<int>(loadedBytes * 100 / fileSize);
        } else {
            return 100;
        }
    } else {
        return 0;
    }
}

void StatusBarWidget::qtSlotLoadProgress(long long loadedBytes, bool done)
{
    const int percent = loadPercent(loadedBytes, fileSize_, done);
    status_ = tr("Loading: %1%").arg(percent);
    updateContent();
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
