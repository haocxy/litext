#pragma once

#include <QMainWindow>

class EditorViewWidget;
class View;
class Editor;
class SimpleDoc;
namespace view
{
    class Config;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:

    MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();

private:
    void initMenuBar();

private:
    view::Config * m_viewConfig = nullptr;
    SimpleDoc * m_doc = nullptr;
    View * m_view = nullptr;
    Editor * m_editor = nullptr;
    EditorViewWidget * m_editorViewWidget = nullptr;
};