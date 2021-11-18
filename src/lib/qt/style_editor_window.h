#pragma once

#include <QMainWindow>

namespace Ui
{
class StyleEditor;
}

namespace gui::qt
{

class StyleEditorWindow : public QMainWindow {
    Q_OBJECT

public:
    StyleEditorWindow();

    virtual ~StyleEditorWindow();

private:
    void initToolBar();

private:
    void openFileAction();

private:
    Ui::StyleEditor *ui_ = nullptr;

    
};

}
