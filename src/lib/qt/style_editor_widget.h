#pragma once

#include <QWidget>

namespace Ui
{
class StyleEditor;
}

namespace gui::qt
{

class StyleEditorWidget : public QWidget {
    Q_OBJECT

public:
    StyleEditorWidget(QWidget *parent = nullptr);

    virtual ~StyleEditorWidget();

private:
    Ui::StyleEditor *ui_ = nullptr;
};

}
