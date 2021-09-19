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

static QValidator *newCharCountValidator(QObject *parent)
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

static void repairRangeByMinEdit(QLineEdit &minEdit, QLineEdit &maxEdit)
{
    const QString text = minEdit.text();
    const int min = text.toInt();
    const int repairedMin = repairCharCountInSingleRow(min);
    if (min != repairedMin) {
        minEdit.setText(QString::number(repairedMin));
    }
    if (maxEdit.text().toInt() < repairedMin) {
        maxEdit.setText(QString::number(repairedMin));
    }
}

static void setupRangeRepairLogicForMinEdit(QLineEdit *minEdit, QLineEdit *maxEdit)
{
    QObject::connect(minEdit, &QLineEdit::editingFinished, minEdit, [minEdit, maxEdit] {
        repairRangeByMinEdit(*minEdit, *maxEdit);
    });
}

static void repairRangeByMaxEdit(QLineEdit &minEdit, QLineEdit &maxEdit)
{
    const QString text = maxEdit.text();
    const int max = text.toInt();
    const int repairedMax = repairCharCountInSingleRow(max);
    if (max != repairedMax) {
        maxEdit.setText(QString::number(repairedMax));
    }
    if (minEdit.text().toInt() > repairedMax) {
        minEdit.setText(QString::number(repairedMax));
    }
}

static void setupRangeRepairLogicForMaxEdit(QLineEdit *minEdit, QLineEdit *maxEdit)
{
    QObject::connect(maxEdit, &QLineEdit::editingFinished, maxEdit, [minEdit, maxEdit] {
        repairRangeByMaxEdit(*minEdit, *maxEdit);
    });
}

void BigFileGeneratorWidget::setupRangeRepairLogic()
{
    QLineEdit *minEdit = ui_->minRowSizeLineEdit;
    QLineEdit *maxEdit = ui_->maxRowSizeLineEdit;
    setupRangeRepairLogicForMinEdit(minEdit, maxEdit);
    setupRangeRepairLogicForMaxEdit(minEdit, maxEdit);

    connect(minEdit, &QLineEdit::textEdited, this, [this](const QString &) {
        lastActRangeEditor_ = ui_->minRowSizeLineEdit;
    });

    connect(maxEdit, &QLineEdit::textEdited, this, [this](const QString &) {
        lastActRangeEditor_ = ui_->maxRowSizeLineEdit;
    });
}

BigFileGeneratorWidget::BigFileGeneratorWidget(QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::BigFileGenerator) {

    ui_->setupUi(this);

    connect(ui_->outputPathChooseButton, &QPushButton::clicked, this, &BigFileGeneratorWidget::openFileChooserDialog);

    initCharsetComboBox(ui_->charsetComboBox);

    ui_->sizeText->setValidator(new QDoubleValidator(this));

    ui_->minRowSizeLineEdit->setValidator(newCharCountValidator(this));
    ui_->maxRowSizeLineEdit->setValidator(newCharCountValidator(this));
    setupRangeRepairLogic();
}

BigFileGeneratorWidget::~BigFileGeneratorWidget()
{
    delete ui_;
    ui_ = nullptr;
}

void BigFileGeneratorWidget::mousePressEvent(QMouseEvent *e)
{
    QWidget::mousePressEvent(e);

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
