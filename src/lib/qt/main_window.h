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

class Application;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(Application &application);

    virtual ~MainWindow();

    virtual void closeEvent(QCloseEvent *e) override;

    void openDocument(const fs::path &file, RowN row);

signals:
    void qtSigShouldReloadStyleSheetFile(QString styleSheetFilePath);

protected:
    virtual void keyPressEvent(QKeyEvent *e) override;

private:
    void saveGeometry();

    void restoreGeometry();

    void initMenuBar();

    void initToolBar();

    void bind(QMenu *menu, const QString &name, void(MainWindow:: *f)());

    void bind(QMenu *menu, const QString &name, void(MainWindow:: *f)(), const QKeySequence &shortcut);

    void bind(QToolBar *toolBar, const QString &name, void(MainWindow:: *f)());

    QToolBar *addToolBar(const QString &title);

private: // Action 统一放在这里

    // 这些由某个按钮或菜单触发的操作统一以 Action 结尾
    // 这样做是为了与那些实际执行逻辑的函数区分
    // 
    // 例如对于 fileOpenAction() , 如果只写成 fileOpen()
    // 就会让人产生 "为什么没有用参数指明要打开哪个文件" 之类的疑问
    //
    // 以 Action 为后缀, 就表明这只是一个 GUI 动作, 并不执行实际的逻辑

    void windowCloseAction();

    void fileOpenAction();

    void viewJumpAction();

    void makeBigFileAction();

    void openStyleEditorAction();

    void reloadStyleSheetFileAction();

private:

    void curEditorPageUp();

    void curEditorPageDown();

private:
    Application &application_;
    Config &config_;
    PropRepo propRepo_;
    QPointer<EditorStackWidget> editorStack_;
};

}

