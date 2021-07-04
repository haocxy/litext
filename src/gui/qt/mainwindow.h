#pragma once

#include <QMainWindow>

#include "core/fs.h"
#include "gui/declare_text_area.h"


class EditorViewWidget;

namespace gui
{
class TextAreaConfig;
}

class Editor;
class SimpleDoc;


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:

    explicit MainWindow(fs::path filePath, QWidget *parent = nullptr);
    virtual ~MainWindow();

private:
    void initMenuBar();

private:
    gui::TextAreaConfig * m_viewConfig = nullptr;
    SimpleDoc * m_doc = nullptr;
    gui::TextArea * m_view = nullptr;
    Editor * m_editor = nullptr;
    EditorViewWidget * m_editorViewWidget = nullptr;
};
