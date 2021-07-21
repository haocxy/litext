#pragma once

#include <QMainWindow>

#include "core/fs.h"
#include "core/strand.h"
#include "core/single_thread_strand_pool.h"
#include "gui/declare_text_area_config.h"
#include "declare_editor_widget.h"
#include "gui_strand.h"


namespace gui::qt
{

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(TextAreaConfig &textAreaConfig, const fs::path &filePath);

    virtual ~MainWindow();

protected:
    virtual void keyReleaseEvent(QKeyEvent *e) override;

private:
    void initMenuBar();

private:
    GuiStrand guiStrand_;
    SingleThreadStrandPool strandPool_;
    TextAreaConfig &textAreaConfig_;
    EditorViewWidget *editorWidget_ = nullptr;
};

}

