#pragma once

#include <QMainWindow>

#include "core/fs.h"
#include "doc/doc_define.h"
#include "gui/declare_config.h"
#include "declare_editor_widget.h"


namespace gui::qt
{

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(Config &config);

    virtual ~MainWindow();

    void openDocument(const fs::path &file, RowN row);

protected:
    virtual void keyReleaseEvent(QKeyEvent *e) override;

private:
    void initMenuBar();

    void initToolBar();

private slots:
    void fileMenuOpenActionTriggered();

private:
    Config &config_;
    EditorWidget *editorWidget_ = nullptr;
};

}

