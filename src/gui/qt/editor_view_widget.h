#pragma once

#include <QWidget>

#include "gui/declare_text_area.h"


namespace gui::qt
{


class RulerWidget;
class TextAreaWidget;
class StatusBarWidget;


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
