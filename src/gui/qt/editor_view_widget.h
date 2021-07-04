#pragma once

#include <QWidget>

#include "gui/declare_text_area.h"
#include "declare_ruler_widget.h"
#include "declare_text_area_widget.h"
#include "declare_status_bar_widget.h"


namespace gui::qt
{


class EditorViewWidget : public QWidget {
    Q_OBJECT
public:
    EditorViewWidget(TextArea *view, QWidget *parent = nullptr);
    virtual ~EditorViewWidget();

private:
    RulerWidget *ruler_ = nullptr;
    TextAreaWidget *textArea_ = nullptr;
    StatusBarWidget *statusBar_ = nullptr;
};


}
