#pragma once

#include <QWidget>

class LineNumBar;
class TextPad;
class View;

class EditorViewWidget : public QWidget
{
    Q_OBJECT
public:
    EditorViewWidget(View * view, QWidget * parent = nullptr);
    virtual ~EditorViewWidget();

private:
    LineNumBar * m_lineNumBar = nullptr;
    TextPad * m_textPad = nullptr;
};
