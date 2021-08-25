#pragma once

#include <QAction>
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

    template <typename R>
    void bind(QMenu *menu, const QString &name, R(MainWindow:: *f)())
    {
        QAction *action = menu->addAction(name);
        connect(action, &QAction::triggered, this, f);
    }

private slots:
    void fileMenuOpenActionTriggered();

    void viewMenuGoLineActionTriggered();

private:
    Config &config_;
    EditorWidget *editorWidget_ = nullptr;
};

}

