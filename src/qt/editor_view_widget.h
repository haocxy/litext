#pragma once

#include <QWidget>

class LineNumBarWidget;
class TextAreaWidget;
class View;
class ViewStatusBar;


class EditorViewWidget : public QWidget
{
    Q_OBJECT
public:
    EditorViewWidget(View * view, QWidget * parent = nullptr);
    virtual ~EditorViewWidget();

private:
    LineNumBarWidget * m_lineNumBar = nullptr;
    TextAreaWidget * textArea_ = nullptr;
	ViewStatusBar * m_viewStatusBar = nullptr;
};
