#pragma once

#include <QMenu>
#include <QAction>
#include <QPointer>
#include <QMainWindow>

#include "core/fs.h"
#include "doc/doc_define.h"
#include "gui/engine.h"
#include "gui/declare_config.h"
#include "declare_editor_widget.h"
#include "editor_stack_widget.h"


namespace gui::qt
{

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(Engine &engine, Config &config);

    virtual ~MainWindow();

    virtual void closeEvent(QCloseEvent *e) override;

    void openDocument(const fs::path &file, RowN row);

protected:
    virtual void keyPressEvent(QKeyEvent *e) override;

private:
    void saveGeometry();

    void restoreGeometry();

    void initMenuBar();

    void initToolBar();

    template <typename T, typename R>
    void bind(QMenu *menu, const QString &name, R(T:: *f)())
    {
        QAction *action = menu->addAction(name);
        connect(action, &QAction::triggered, this, f);
    }

private:
    void fileMenuOpenActionTriggered();

    void viewMenuGoLineActionTriggered();

    void makeBigFileActionTriggered();

    void curEditorPageUp();

    void curEditorPageDown();

private:
    Engine &engine_;
    Config &config_;
    PropRepo propRepo_;
    QPointer<EditorStackWidget> editorStack_;
};

}

