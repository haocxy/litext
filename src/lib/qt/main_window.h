#pragma once

#include <QMenu>
#include <QToolBar>
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

    template <typename T, typename R>
    void bind(QToolBar *toolBar, const QString &name, R(T:: *f)())
    {
        QAction *action = toolBar->addAction(name);
        connect(action, &QAction::triggered, this, f);
    }

    QToolBar *addToolBar(const QString &title);

private: // Action 统一放在这里

    // 这些由某个按钮或菜单触发的操作统一以 Action 结尾
    // 这样做是为了与那些实际执行逻辑的函数区分
    // 
    // 例如对于 fileOpenAction() , 如果只写成 fileOpen()
    // 就会让人产生 "为什么没有用参数指明要打开哪个文件" 之类的疑问
    //
    // 以 Action 为后缀, 就表明这只是一个 GUI 动作, 并不执行实际的逻辑

    void fileOpenAction();

    void viewJumpAction();

    void makeBigFileAction();

private:

    void curEditorPageUp();

    void curEditorPageDown();

private:
    Engine &engine_;
    Config &config_;
    PropRepo propRepo_;
    QPointer<EditorStackWidget> editorStack_;
};

}

