#pragma once

#include <QWidget>

#include "core/fs.h"
#include "core/sigconns.h"
#include "editor/editor.h"
#include "gui/text_area.h"
#include "gui/declare_text_area_config.h"
#include "declare_ruler_widget.h"
#include "declare_text_area_widget.h"
#include "declare_status_bar_widget.h"
#include "text_area_scrollbar.h"


namespace gui::qt
{


class EditorWidget : public QWidget {
    Q_OBJECT
public:
    EditorWidget(const TextAreaConfig &textAreaConfig, sptr<Editor> editor, RowN row);

    virtual ~EditorWidget();

    const TextAreaWidget &textAreaWidget() const;

    const doc::Document &document() const;
    
    void jumpTo(RowN row);

signals:
    void qtSigDocFatalError(int err);

    void qtSigUpdateScrollBarMaximum(int maximum);

private:
    QString docErrToStr(doc::DocError err) const;

    void handleDocFatalError(int err);

private:
    sptr<Editor> editor_;
    TextArea textArea_;
    RulerWidget *ruler_ = nullptr;
    TextAreaWidget *textAreaWidget_ = nullptr;
    TextAreaScrollBar *vScrollBar_ = nullptr;
    StatusBarWidget *statusBar_ = nullptr;
    SigConns sigConns_;
};


}
