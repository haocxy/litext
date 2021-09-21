#pragma once

#include <optional>

#include <QWidget>
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

class EditorStatusBarWidget : public QWidget {
    Q_OBJECT
public:
    explicit EditorStatusBarWidget(TextArea &textArea, QWidget *parent = nullptr);

    virtual ~EditorStatusBarWidget();

protected:
    virtual void paintEvent(QPaintEvent *e) override;

private:
    void initCharsetMenu();

private:
signals:
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
    QPointer<QMenu> charsetMenu_;
};

}
