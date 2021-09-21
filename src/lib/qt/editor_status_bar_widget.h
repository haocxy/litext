#pragma once

#include <optional>

#include <QWidget>

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

private:
signals:
    void qtSigFileSizeDetected(long long fileSize);

    void qtSigRowCountUpdated(long long rowCount);

    void qtSigCharsetDetected(QString charset);

    void qtSigCharsetDetectFailed();

private:
    TextArea &textArea_;
    Ui::EditorStatusBar *ui_ = nullptr;
    SigConns sigConns_;
    std::optional<long long> fileSize_;
};

}
