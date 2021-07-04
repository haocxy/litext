#pragma once

#include <QMainWindow>

#include "core/fs.h"


class EditorViewWidget;
class TextArea;
class Editor;
class SimpleDoc;
namespace view
{
    class TextAreaConfig;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:

    explicit MainWindow(fs::path filePath, QWidget *parent = nullptr);
    virtual ~MainWindow();

private:
    void initMenuBar();

private:
    view::TextAreaConfig * m_viewConfig = nullptr;
    SimpleDoc * m_doc = nullptr;
    TextArea * m_view = nullptr;
    Editor * m_editor = nullptr;
    EditorViewWidget * m_editorViewWidget = nullptr;
};
