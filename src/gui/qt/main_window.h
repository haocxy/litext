#pragma once

#include <QMainWindow>

#include "core/fs.h"
#include "core/strand.h"
#include "core/single_thread_strand_pool.h"
#include "gui/declare_text_area.h"
#include "gui/declare_text_area_config.h"
#include "declare_editor_view_widget.h"
#include "gui_strand.h"


class Editor;
class SimpleDoc;


namespace gui::qt
{


class MainWindow : public QMainWindow {
    Q_OBJECT
public:

    MainWindow(TextAreaConfig &textAreaConfig, const fs::path &filePath, QWidget *parent = nullptr);

    virtual ~MainWindow();

protected:
    virtual void keyReleaseEvent(QKeyEvent *e) override;

private:
    void initMenuBar();

private:
    GuiStrand guiStrand_;
    SingleThreadStrandPool strandPool_;
    TextAreaConfig &textAreaConfig_;
    SimpleDoc *m_doc = nullptr;
    TextArea *m_view = nullptr;
    Editor *editor_ = nullptr;
    EditorViewWidget *m_editorViewWidget = nullptr;
};


}
