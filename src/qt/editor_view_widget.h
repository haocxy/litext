#pragma once

#include <QWidget>


class RulerWidget;
class TextAreaWidget;
class View;
class StatusBarWidget;


class EditorViewWidget : public QWidget
{
    Q_OBJECT
public:
    EditorViewWidget(View * view, QWidget * parent = nullptr);
    virtual ~EditorViewWidget();

private:
    RulerWidget *ruler_ = nullptr;
    TextAreaWidget *textArea_ = nullptr;
    StatusBarWidget *statusBar_ = nullptr;
};
