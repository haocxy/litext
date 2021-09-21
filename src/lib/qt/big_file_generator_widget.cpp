#include "big_file_generator_widget.h"

#include <cstdlib>

#include <set>
#include <fstream>
#include <sstream>

#include <QFileDialog>
#include <QMessageBox>
#include <QTextCodec>

#include "core/system.h"

#include "qtutil.h"
#include "gen.big_file_generator.ui.h"


namespace gui::qt
{

static void initCharsetComboBox(QComboBox *box)
{
    std::set<QString> charsetNameSet = util::supportedCharsetSet();
    charsetNameSet.erase("ASCII");

    const QStringList charsetNameList = util::toQList(charsetNameSet);

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

static const int kDefaultCharCountInSingleRow = 100;


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

    ui_->fileSizeText->setValidator(new QDoubleValidator(this));

    ui_->minRowSizeLineEdit->setValidator(newCharCountValidator(this));
    ui_->maxRowSizeLineEdit->setValidator(newCharCountValidator(this));

    connect(ui_->executeButton, &QPushButton::clicked, this, &BigFileGeneratorWidget::executeTriggered);

    connect(this, &BigFileGeneratorWidget::generateProgress, this, [this](int percent) {
        ui_->progressBar->setValue(percent);
    });

    connect(this, &BigFileGeneratorWidget::generateDone, this, [this](QString path) {
        generator_ = nullptr;
        ui_->executeButton->setEnabled(true);
        ui_->progressBar->setValue(100);
        if (ui_->openAfterGeneratedCheckBox->isChecked()) {
            emit shouldOpenGeneratedFile(path);
        }
    });

    connect(this, &BigFileGeneratorWidget::generateError, this, [this] (const QString &msg) {
        error(msg);
        generator_ = nullptr;
        ui_->executeButton->setEnabled(true);
    });
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

    if (!outpath.isEmpty()) {
        ui_->outputPathLineEdit->setText(outpath);
    }
}

void BigFileGeneratorWidget::executeTriggered()
{
    const std::optional<fs::path> outputPath = getOutputPath();
    if (!outputPath) {
        return;
    }

    const std::optional<QString> outputCharset = getOutputCharset();
    if (!outputCharset) {
        return;
    }

    const std::optional<i64> outputFileSize = getOutputFileSize();
    if (!outputFileSize) {
        return;
    }

    const std::optional<RowSizeRange> outputRowSizeRange = getRowSizeRange();
    if (!outputRowSizeRange) {
        return;
    }

    ui_->executeButton->setEnabled(false);
    ui_->progressBar->setValue(0);
    setFocus();

    GenerateParam param;
    param.path = *outputPath;
    param.charset = *outputCharset;
    param.filesize = *outputFileSize;
    param.rowSizeRange = *outputRowSizeRange;

    generator_ = std::make_unique<Generator>(*this, param);

    generator_->start();
}

bool BigFileGeneratorWidget::confirm(const QString &text)
{
    const QMessageBox::StandardButtons buttons =
        QMessageBox::Ok | QMessageBox::Cancel;

    QMessageBox::StandardButton result = QMessageBox::warning(
        this, windowTitle(), text, buttons
    );

    switch (result) {
    case QMessageBox::Ok:
        return true;
    default:
        return false;
    }
}

void BigFileGeneratorWidget::error(const QString &text)
{
    QMessageBox::critical(
        this, windowTitle(), text, QMessageBox::Ok
    );
}

bool BigFileGeneratorWidget::confirmContinue(const QString &text)
{
    const QString newLine = tr("\n");
    QString confirmText;
    confirmText += tr("Warning! ");
    confirmText += newLine;
    confirmText += newLine;
    confirmText += text;
    confirmText += newLine;
    confirmText += newLine;
    confirmText += tr("Your confirm is needed.");
    return confirm(confirmText);
}

std::optional<fs::path> BigFileGeneratorWidget::getOutputPath()
{
    const QString qpath = ui_->outputPathLineEdit->text();
    if (qpath.isEmpty()) {
        error(tr("Please specify an output file."));
        return std::nullopt;
    }

    const fs::path path = qpath.toStdU32String();
    if (!path.is_absolute()) {
        error(tr("Please use absolute path for your file's safe."));
        return std::nullopt;
    }

    if (!fs::exists(path)) {
        return path;
    }

    if (fs::is_directory(path)) {
        error(tr("Path of output file [%1] is a directory.").arg(qpath));
        return std::nullopt;
    }

    if (!fs::is_regular_file(path)) {
        error(tr("Output file [ %1 ] is not a regular file.").arg(qpath));
        return std::nullopt;
    }

    if (!confirmContinue(tr("Output file [ %1 ] already existed.\nIt's content will be deleted if you confirm.").arg(qpath))) {
        return std::nullopt;
    }

    return path;
}

std::optional<QString> BigFileGeneratorWidget::getOutputCharset()
{
    return ui_->charsetComboBox->currentText();
}

std::optional<i64> BigFileGeneratorWidget::getOutputFileSize()
{
    const QString sizeText = ui_->fileSizeText->text();
    if (sizeText.isEmpty()) {
        error(tr("Please specify the size of output file."));
        return std::nullopt;
    }

    const bool gbChecked = ui_->fileSizeUnitGbButton->isChecked();
    const bool mbChecked = ui_->fileSizeUnitMbButton->isChecked();

    if (gbChecked && mbChecked) {
        error(tr("Please check only one unit: [GB] or [MB]."));
        return std::nullopt;
    }

    if (!gbChecked && !mbChecked) {
        error(tr("Please check unit."));
        return std::nullopt;
    }

    double num = sizeText.toDouble();

    constexpr double KB = 1024;
    constexpr double MB = 1024 * KB;
    constexpr double GB = 1024 * MB;

    if (gbChecked) {
        return num * GB;
    } else if (mbChecked) {
        return num * MB;
    } else {
        return std::nullopt;
    }
}

std::optional<BigFileGeneratorWidget::RowSizeRange> BigFileGeneratorWidget::getRowSizeRange()
{
    const QString minText = ui_->minRowSizeLineEdit->text();
    if (minText.isEmpty()) {
        error(tr("Please specify minimum value for row size"));
        return std::nullopt;
    }

    const i64 min = minText.toInt();
    if (min < kMinCharCountInSingleRow) {
        error(tr("Values of row size should be at least %1").arg(kMinCharCountInSingleRow));
        return std::nullopt;
    }

    const QString maxText = ui_->maxRowSizeLineEdit->text();
    if (maxText.isEmpty()) {
        error(tr("Please specify maximum value for row size"));
        return std::nullopt;
    }

    const i64 max = maxText.toInt();
    if (max < kMinCharCountInSingleRow) {
        error(tr("Values of row size should be at least %1").arg(kMinCharCountInSingleRow));
        return std::nullopt;
    }

    if (min > max) {
        error(tr("Minimum value of row size should not be greater than maximum value"));
        return std::nullopt;
    }

    return RowSizeRange::byLeftAndRight(min, max);
}

BigFileGeneratorWidget::Generator::Generator(BigFileGeneratorWidget &gui, const GenerateParam &param)
    : gui_(gui)
    , param_(param) {}

BigFileGeneratorWidget::Generator::~Generator()
{
    stopping_ = true;

    if (thread_.joinable()) {
        thread_.join();
    }
}

void BigFileGeneratorWidget::Generator::start()
{
    thread_ = std::thread([this] {
        generate();
    });
}

void BigFileGeneratorWidget::Generator::generate()
{
    std::srand(std::time(nullptr));

    prepareCharPool();

    std::ofstream ofs(param_.path, std::ios::binary);
    if (!ofs) {
        emit gui_.generateError(
            tr("Cannot open file [ %1 ] for write.")
            .arg(QString::fromStdU32String(param_.path.generic_u32string())));

        return;
    }

    QTextCodec *outCodec = QTextCodec::codecForName(param_.charset.toStdString().c_str());
    if (!outCodec) {
        emit gui_.generateError(tr("Cannot make codec for charset [ %1 ].").arg(param_.charset));
        return;
    }

    uptr<QTextEncoder> outEncoder(outCodec->makeEncoder());
    if (!outEncoder) {
        emit gui_.generateError(tr("Cannot make encoder for charset [ %1 ].").arg(param_.charset));
        return;
    }

    int prevPercent = 0;

    for (i64 rowIndex = 0; ; ++rowIndex) {
        const i64 written = ofs.tellp();
        if (written >= param_.filesize) {
            break;
        }
        if (stopping_) {
            return; // 提前结束时不报告进度,直接以当前状态结束
        }

        const std::u32string row = randRow(rowIndex);
        QByteArray qbytes = outEncoder->fromUnicode(QString::fromStdU32String(row));
        ofs.write(qbytes.constData(), qbytes.size());

        const int percent = static_cast<int>(written * 100 / param_.filesize);
        if (percent > prevPercent) {
            emit gui_.generateProgress(percent);
        }
        prevPercent = percent;
    }

    emit gui_.generateProgress(100);
    emit gui_.generateDone(QString::fromStdU32String(param_.path.generic_u32string()));
}

void BigFileGeneratorWidget::Generator::prepareCharPool()
{
    QFile qfile(":/char_pool/chinese.txt");
    qfile.open(QIODevice::ReadOnly);

    QByteArray qbytes = qfile.readAll();

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    if (!codec) {
        emit gui_.generateError(tr("Cannot make codec for UTF-8"));
        return;
    }

    uptr<QTextDecoder> decoder(codec->makeDecoder());
    if (!decoder) {
        emit gui_.generateError(tr("Cannot make decoder for UTF-8"));
        return;
    }

    QString qstr = decoder->toUnicode(qbytes);
    std::u32string u32s = qstr.toStdU32String();

    for (char32_t c : u32s) {
        if (c != '\r' && c != '\n') {
            charPool_.push_back(c);
        }
    }
}

static const std::u32string kRowEndStr = U"\r\n";

std::u32string BigFileGeneratorWidget::Generator::randRow(i64 rowIndex)
{
    std::u32string result;

    const i64 rowSize = randRowSize();

    result.reserve(rowSize + kRowEndStr.size());

    for (i64 i = 0; i < rowSize; ++i) {
        result.push_back(randChar());
    }

    result.append(mkRowNumLabel(rowIndex));
    result.append(kRowEndStr);

    return result;
}

i64 BigFileGeneratorWidget::Generator::randRowSize()
{

    const i64 left = param_.rowSizeRange.left();
    const i64 right = param_.rowSizeRange.right();

    //    [left, right]
    // => left + [0, right - left]
    // => left + rand() % (right - left + 1)

    if (kMinCharCountInSingleRow <= left && left <= right) {
        return left + std::rand() % (right - left + 1);
    } else {
        return kDefaultCharCountInSingleRow;
    }
}

char32_t BigFileGeneratorWidget::Generator::randChar()
{
    return charPool_[rand() % charPool_.size()];
}

std::u32string BigFileGeneratorWidget::Generator::mkRowNumLabel(i64 rowIndex)
{
    std::ostringstream ss;
    ss << "[" << (rowIndex + 1) << "]";

    std::u32string result;
    for (char c : ss.str()) {
        result.push_back(c);
    }

    return result;
}

}
