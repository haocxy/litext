#include "big_file_generator_widget.h"

#include <QFileDialog>

#include "core/system.h"

#include "gen.big_file_generator.ui.h"


namespace gui::qt
{

BigFileGeneratorWidget::BigFileGeneratorWidget(QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::BigFileGenerator) {

    ui_->setupUi(this);

    connect(ui_->outputPathChooseButton, &QPushButton::clicked, this, &BigFileGeneratorWidget::openFileChooserDialog);

    updateSizeSliderRangeLabels(ui_->sizeSlider->minimum(), ui_->sizeSlider->maximum());
    connect(ui_->sizeSlider, &QSlider::rangeChanged, this, &BigFileGeneratorWidget::updateSizeSliderRangeLabels);
}

BigFileGeneratorWidget::~BigFileGeneratorWidget()
{
    delete ui_;
    ui_ = nullptr;
}

void BigFileGeneratorWidget::openFileChooserDialog()
{
    const std::u32string userHome = SystemUtil::userHome().generic_u32string();
    const QString defaultDir = QString::fromStdU32String(userHome);
    QString outpath = QFileDialog::getSaveFileName(this, tr("Choose Output File"), defaultDir);

    ui_->outputPathLineEdit->setText(outpath);
}

void BigFileGeneratorWidget::updateSizeSliderRangeLabels(int min, int max)
{
    ui_->labelSizeSliderMin->setText(QString::number(min));
    ui_->labelSizeSliderMax->setText(QString::number(max));
}

}
