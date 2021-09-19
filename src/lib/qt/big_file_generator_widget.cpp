#include "big_file_generator_widget.h"

#include <set>

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

static std::set<QString> getCharsetNames(bool onlyShowSupportedCharsets)
{
    std::set<QString> result;

    if (onlyShowSupportedCharsets) {
        for (const Charset charset : doc::gSupportedCharsets) {
            result.insert(CharsetUtil::charsetToStr(charset));
        }
    } else {
        const QByteArrayList codecByteArrays = QTextCodec::availableCodecs();
        for (const QByteArray &codecByteArray : codecByteArrays) {
            result.insert(codecByteArray);
        }
    }

    result.erase("ASCII");

    return result;
}

static QStringList toQStringList(const std::set<QString> &charsetNames)
{
    QStringList result;
    for (const QString &charsetName : charsetNames) {
        result.push_back(charsetName);
    }
    return result;
}

void BigFileGeneratorWidget::updateCharsetComboBox()
{
    ui_->charsetComboBox->clear();

    const std::set<QString> charsetNameSet = getCharsetNames(ui_->onlyShowSupportedCharsetsCheckBox->isChecked());

    const QStringList charsetNameList = toQStringList(charsetNameSet);

    ui_->charsetComboBox->addItems(charsetNameList);

    const QString defaultCharset = "UTF-8";
    if (charsetNameSet.find(defaultCharset) != charsetNameSet.end()) {
        ui_->charsetComboBox->setCurrentText(defaultCharset);
    }
}

void BigFileGeneratorWidget::updateSizeSliderRangeLabels(int min, int max)
{
    ui_->labelSizeSliderMin->setText(QString::number(min));
    ui_->labelSizeSliderMax->setText(QString::number(max));
}

}
