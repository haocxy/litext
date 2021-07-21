#pragma once

#include <QMainWindow>

#include "core/fs.h"
#include "core/strand.h"
#include "core/single_thread_strand_pool.h"
#include "gui/declare_config.h"
#include "declare_editor_widget.h"
#include "gui_strand.h"


namespace gui::qt
{

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(Config &config);

    MainWindow(Config &config, const fs::path &filePath);

    virtual ~MainWindow();

    void openDocument(const fs::path &file);

protected:
    virtual void keyReleaseEvent(QKeyEvent *e) override;

private:
    void initMenuBar();

    void initToolBar();

private slots:
    void fileMenuOpenActionTriggered();

private:
    GuiStrand guiStrand_;
    SingleThreadStrandPool strandPool_;
    Config &config_;
    EditorWidget *editorWidget_ = nullptr;
};

}

