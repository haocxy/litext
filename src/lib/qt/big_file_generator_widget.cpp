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

static QIntValidator *newCharCountValidator(QObject *parent)
{
    QIntValidator *v = new QIntValidator(parent);
    v->setBottom(0);
    return v;
}

static const int kMinCharCountInSingleRow = 1;

static int repairCharCountInSingleRow(int value)
{
    if (value >= kMinCharCountInSingleRow) {
        return value;
    } else {
        return kMinCharCountInSingleRow;
    }
}

BigFileGeneratorWidget::BigFileGeneratorWidget(QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::BigFileGenerator) {

    ui_->setupUi(this);

    connect(ui_->outputPathChooseButton, &QPushButton::clicked, this, &BigFileGeneratorWidget::openFileChooserDialog);

    initCharsetComboBox(ui_->charsetComboBox);

    ui_->sizeText->setValidator(new QDoubleValidator(this));

    ui_->minRowSizeLineEdit->setValidator(newCharCountValidator(this));
    connect(ui_->minRowSizeLineEdit, &QLineEdit::editingFinished, this, [this] {
        const QString text = ui_->minRowSizeLineEdit->text();
        const int min = text.toInt();
        const int repairedMin = repairCharCountInSingleRow(min);
        if (min != repairedMin) {
            ui_->minRowSizeLineEdit->setText(QString::number(repairedMin));
        }
        if (ui_->maxRowSizeLineEdit->text().toInt() < repairedMin) {
            ui_->maxRowSizeLineEdit->setText(QString::number(repairedMin));
        }
    });

    ui_->maxRowSizeLineEdit->setValidator(newCharCountValidator(this));
    connect(ui_->maxRowSizeLineEdit, &QLineEdit::editingFinished, this, [this] {
        const QString text = ui_->maxRowSizeLineEdit->text();
        const int max = text.toInt();
        const int repairedMax = repairCharCountInSingleRow(max);
        if (max != repairedMax) {
            ui_->maxRowSizeLineEdit->setText(QString::number(repairedMax));
        }
        if (ui_->minRowSizeLineEdit->text().toInt() > repairedMax) {
            ui_->minRowSizeLineEdit->setText(QString::number(repairedMax));
        }
    });
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
