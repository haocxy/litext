#pragma once

#include <QMainWindow>

namespace Ui
{
class StyleEditor;
}

namespace gui::qt
{

class Application;

class StyleEditorWindow : public QMainWindow {
    Q_OBJECT

public:
    StyleEditorWindow(Application &application);

    virtual ~StyleEditorWindow();

private:
    void initToolBar();

private:
    void openFileAction();

private:
    Ui::StyleEditor *ui_ = nullptr;
    Application &application_;
};

}
