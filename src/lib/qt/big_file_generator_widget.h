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

private:
    void openFileChooserDialog();

    void updateSizeSliderRangeLabels(int min, int max);

private:
    Ui::BigFileGenerator *ui_ = nullptr;
};

}
