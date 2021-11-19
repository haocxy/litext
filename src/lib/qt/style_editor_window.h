#pragma once

#include <QMainWindow>

namespace Ui
{
class StyleEditor;
}

namespace gui::qt
{

class Application;

class StyleEditWidget;

class StyleEditorWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit StyleEditorWindow(Application &application);

    virtual ~StyleEditorWindow();

private:
    void initToolBar();

private:
    void noAction() {}

    void openFileAction();

    void applyAction();

private:
    Application &application_;
    Ui::StyleEditor *ui_ = nullptr;
    StyleEditWidget *styleEdit_ = nullptr;
};

}
