#pragma once

#include <optional>

#include <QFrame>
#include <QMenu>
#include <QPointer>

#include "core/sigconns.h"
#include "gui/declare_text_area.h"


namespace Ui
{
class EditorStatusBar;
}

namespace gui::qt
{

class EditorStatusBarWidget : public QFrame {
    Q_OBJECT
public:
    explicit EditorStatusBarWidget(TextArea &textArea, QWidget *parent = nullptr);

    virtual ~EditorStatusBarWidget();

private:
    void initCharsetMenu();

    void onStartLoad();

    void showStatusMessage(const QString &statusType, const QString &message);

    void setStatusType(const QString &statusType);

    void setStatus(const QString &statusType);

    void clearStatusMessage();

private:
signals:
    void qtSigStartLoad();

    void qtSigFileSizeDetected(long long fileSize);

    void qtSigRowCountUpdated(long long rowCount);

    void qtSigCharsetDetected(QString charset);

    void qtSigCharsetDetectFailed();

    void qtSigLoadProgress(long long loadedBytes, bool done);

    void qtSigLoadDone(long long timeUsageMs);

private:
    TextArea &textArea_;
    Ui::EditorStatusBar *ui_ = nullptr;
    SigConns sigConns_;
    std::optional<long long> fileSize_;
    long long maxRowCount_ = 0;
    QPointer<QMenu> charsetMenu_;
};

}
