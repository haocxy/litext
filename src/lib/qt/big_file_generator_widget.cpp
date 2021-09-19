#include "big_file_generator_widget.h"

#include <set>

#include <QFileDialog>
#include <QTextCodec>

#include "core/system.h"
#include "doc/supported_charsets.h"

#include "gen.big_file_generator.ui.h"


namespace gui::qt
{

static std::set<QString> getCharsetNames()
{
    std::set<QString> result;

    for (const Charset charset : doc::gSupportedCharsets) {
        if (charset != Charset::Ascii) {
            result.insert(CharsetUtil::charsetToStr(charset));
        }
    }

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

static void initCharsetComboBox(QComboBox *box)
{
    const std::set<QString> charsetNameSet = getCharsetNames();

    const QStringList charsetNameList = toQStringList(charsetNameSet);

    box->addItems(charsetNameList);

    const QString defaultCharset = "UTF-8";
    if (charsetNameSet.find(defaultCharset) != charsetNameSet.end()) {
        box->setCurrentText(defaultCharset);
    }
}

BigFileGeneratorWidget::BigFileGeneratorWidget(QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::BigFileGenerator) {

    ui_->setupUi(this);

    connect(ui_->outputPathChooseButton, &QPushButton::clicked, this, &BigFileGeneratorWidget::openFileChooserDialog);

    initCharsetComboBox(ui_->charsetComboBox);

    ui_->sizeText->setValidator(new QDoubleValidator(this));
}

BigFileGeneratorWidget::~BigFileGeneratorWidget()
{
    delete ui_;
    ui_ = nullptr;
}

void BigFileGeneratorWidget::mousePressEvent(QMouseEvent *e)
{
    setFocus();
}

void BigFileGeneratorWidget::openFileChooserDialog()
{
    const std::u32string userHome = SystemUtil::userHome().generic_u32string();
    const QString defaultDir = QString::fromStdU32String(userHome);
    QString outpath = QFileDialog::getSaveFileName(this, tr("Choose Output File"), defaultDir);

    ui_->outputPathLineEdit->setText(outpath);
}



}
