#include "big_file_generator_widget.h"

#include <QFileDialog>
#include <QTextCodec>

#include "core/system.h"
#include "doc/supported_charsets.h"

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

    updateCharsetComboBox();
    connect(ui_->onlyShowSupportedCharsetsCheckBox, &QCheckBox::stateChanged, this, [this](int state) {
        updateCharsetComboBox();
    });
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

void BigFileGeneratorWidget::updateCharsetComboBox()
{
    ui_->charsetComboBox->clear();

    if (ui_->onlyShowSupportedCharsetsCheckBox->isChecked()) {
        for (const Charset charset : doc::gSupportedCharsets) {
            ui_->charsetComboBox->addItem(CharsetUtil::charsetToStr(charset));
        }
        const Charset defaultCharset = Charset::UTF_8;
        if (doc::gSupportedCharsets.find(defaultCharset) != doc::gSupportedCharsets.end()) {
            ui_->charsetComboBox->setCurrentText(CharsetUtil::charsetToStr(defaultCharset));
        }
    } else {
        const QByteArrayList codecs = QTextCodec::availableCodecs();
        for (const QByteArray &codec : codecs) {
            QString name = codec.fromRawData(codec.constData(), codec.size());
            ui_->charsetComboBox->addItem(name);
        }
        const QString defaultCharset = "UTF-8";
        ui_->charsetComboBox->setCurrentText(defaultCharset);
    }
}

void BigFileGeneratorWidget::updateSizeSliderRangeLabels(int min, int max)
{
    ui_->labelSizeSliderMin->setText(QString::number(min));
    ui_->labelSizeSliderMax->setText(QString::number(max));
}

}
