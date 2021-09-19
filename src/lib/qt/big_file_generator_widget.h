#pragma once

#include <QWidget>

namespace Ui
{
class BigFileGenerator;
}

namespace gui::qt
{

class BigFileGeneratorWidget : public QWidget {
    Q_OBJECT
public:
    BigFileGeneratorWidget(QWidget *parent = nullptr);

    virtual ~BigFileGeneratorWidget();

protected:
    virtual void mousePressEvent(QMouseEvent *e) override;

private:
    void openFileChooserDialog();

    void updateCharsetComboBox();

private:
    Ui::BigFileGenerator *ui_ = nullptr;
};

}
