#include "editor_status_bar_widget.h"

#include <QPainter>

#include "gui/text_area.h"
#include "editor/editor.h"

#include "qtutil.h"
#include "gen.editor_status_bar.ui.h"

namespace gui::qt
{

static QString readableFileSizeHelp(double fileSize, double unitValue, QString unitText)
{
    return QString("%1 %2").arg(fileSize / unitValue, 0, 'f', 2).arg(unitText);
}

static QString readableFileSize(double fileSize)
{
    constexpr double KB = 1024;
    constexpr double MB = KB * 1024;
    constexpr double GB = MB * 1024;

    if (fileSize >= GB) {
        return readableFileSizeHelp(fileSize, GB, "GB");
    }
    if (fileSize >= MB) {
        return readableFileSizeHelp(fileSize, MB, "MB");
    }
    if (fileSize >= KB) {
        return readableFileSizeHelp(fileSize, KB, "KB");
    }
    return readableFileSizeHelp(fileSize, 1, "B");
}

static QString readableRowCount(long long rowCount)
{
    QString s = QString::number(rowCount);

    QString result;

    const int len = s.length();

    result.reserve(len + len / 3);

    int count = 0;

    for (int i = len - 1; i >= 0; --i, ++count) {
        if (count != 0 && count % 3 == 0) {
            result.push_front(',');
        }
        result.push_front(s[i]);
    }

    return result;
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

EditorStatusBarWidget::EditorStatusBarWidget(TextArea &textArea, QWidget *parent)
    : QFrame(parent)
    , textArea_(textArea)
    , ui_(new Ui::EditorStatusBar) {

    ui_->setupUi(this);

    doc::Document &doc = textArea.editor().document();

    using Class = EditorStatusBarWidget;

    sigConns_ += doc.sigFileSizeDetected().connect([this](i64 fileSize) {
        emit qtSigFileSizeDetected(static_cast<long long>(fileSize));
    });

    connect(this, &Class::qtSigFileSizeDetected, this, [this](long long fileSize) {
        fileSize_ = fileSize;
        ui_->fileSizeContent->setText(readableFileSize(static_cast<double>(fileSize)));
    });

    sigConns_ += doc.sigRowCountUpdated().connect([this](RowN rowCount) {
        emit qtSigRowCountUpdated(static_cast<long long>(rowCount));
    });

    connect(this, &Class::qtSigRowCountUpdated, this, [this](long long rowCount) {
        ui_->rowCountContent->setText(readableRowCount(rowCount));
    });

    sigConns_ += doc.sigCharsetDetected().connect([this](Charset charset) {
        if (charset != Charset::Unknown) {
            emit qtSigCharsetDetected(CharsetUtil::charsetToStr(charset));
        }
    });

    connect(this, &Class::qtSigCharsetDetected, this, [this](QString charset) {
        ui_->charsetContent->setText(charset);
    });

    sigConns_ += doc.sigFatalError().connect([this](doc::DocError error) {
        if (error == doc::DocError::UnsupportedCharset) {
            emit qtSigCharsetDetectFailed();
        }
    });

    connect(this, &Class::qtSigCharsetDetectFailed, this, [this] {
        ui_->charsetContent->setText(tr("Unknown"));
    });

    initCharsetMenu();

    ui_->charsetSpecifyButton->setMenu(charsetMenu_);

    sigConns_ += doc.sigLoadProgress().connect([this](const doc::LoadProgress &p) {
        emit qtSigLoadProgress(static_cast<long long>(p.loadedByteCount()), p.done());
    });

    connect(this, &Class::qtSigLoadProgress, this, [this](long long loadedBytes, bool done) {
        if (fileSize_) {
            const int percent = loadPercent(loadedBytes, *fileSize_, done);
            if (percent > ui_->progressBar->value()) {
                ui_->progressBar->setValue(percent);
            }
        }
    });

    sigConns_ += doc.sigAllLoaded().connect([this] {
        const long long timeUsageMs = static_cast<long long>(textArea_.doc().loadTimeUsageMs());
        emit qtSigLoadDone(timeUsageMs);
    });

    connect(this, &Class::qtSigLoadDone, this, [this](long long timeUsageMs) {
        ui_->progressLabel->setText(tr("Loaded by %1 ms").arg(timeUsageMs));
    });

    setStyleSheet("#EditorStatusBar{border-top:1px solid black;}");
}

EditorStatusBarWidget::~EditorStatusBarWidget()
{
    delete ui_;
    ui_ = nullptr;
}

void EditorStatusBarWidget::initCharsetMenu()
{
    charsetMenu_ = new QMenu(this);

    const QList<QString> supportedCharsets = util::supportedCharsetList();

    for (const QString &charset : supportedCharsets) {
        QAction *action = charsetMenu_->addAction(charset);
    }
}

}