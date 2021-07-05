#pragma once

#include <QMainWindow>

#include "core/fs.h"
#include "gui/declare_text_area.h"
#include "gui/declare_text_area_config.h"
#include "declare_editor_view_widget.h"


class Editor;
class SimpleDoc;


namespace gui::qt
{


class MainWindow : public QMainWindow {
    Q_OBJECT
public:

    explicit MainWindow(fs::path filePath, QWidget *parent = nullptr);
    virtual ~MainWindow();

private:
    void initMenuBar();

private:
    TextAreaConfig *m_viewConfig = nullptr;
    SimpleDoc *m_doc = nullptr;
    TextArea *m_view = nullptr;
    Editor *m_editor = nullptr;
    EditorViewWidget *m_editorViewWidget = nullptr;
};


}