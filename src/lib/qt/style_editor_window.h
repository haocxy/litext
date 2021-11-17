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
    Ui::StyleEditor *ui_ = nullptr;
};

}
