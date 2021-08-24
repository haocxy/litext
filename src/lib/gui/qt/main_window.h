#pragma once

#include <QMainWindow>

#include "core/fs.h"
#include "gui/declare_config.h"
#include "declare_editor_widget.h"


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
    Config &config_;
    EditorWidget *editorWidget_ = nullptr;
};

}

