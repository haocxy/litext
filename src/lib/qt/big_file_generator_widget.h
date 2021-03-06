#pragma once

#include <thread>
#include <vector>
#include <optional>

#include <QWidget>

#include "core/fs.h"
#include "core/basetype.h"
#include "core/range.h"


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

signals:
    void shouldOpenGeneratedFile(QString path);

protected:
    virtual void mousePressEvent(QMouseEvent *e) override;

private:
signals:
    void generateProgress(int percent);

    void generateDone(QString path);

    void generateError(const QString &msg);

private:
    void openFileChooserDialog();

    void setupRangeRepairLogic();

    void executeTriggered();

    bool confirm(const QString &text);

    void error(const QString &text);

    bool confirmContinue(const QString &text);

    std::optional<fs::path> getOutputPath();

    std::optional<QString> getOutputCharset();

    std::optional<i64> getOutputFileSize();

    using RowSizeRange = Range<i64>;
    std::optional<RowSizeRange> getRowSizeRange();

    struct GenerateParam {
        fs::path path;
        QString charset;
        i64 filesize;
        RowSizeRange rowSizeRange;
    };

    class Generator {
    public:
        Generator(BigFileGeneratorWidget &gui, const GenerateParam &param);

        ~Generator();

        void start();

    private:
        void generate();

        void prepareCharPool();

        std::u32string randRow(i64 rowIndex);

        i64 randRowSize();

        char32_t randChar();

        std::u32string mkRowNumLabel(i64 rowIndex);

    private:
        BigFileGeneratorWidget &gui_;
        GenerateParam param_;
        std::atomic_bool stopping_;
        std::thread thread_;

    private:
        std::vector<char32_t> charPool_;
    };

private:
    Ui::BigFileGenerator *ui_ = nullptr;
    QWidget *lastActRangeEditor_ = nullptr;
    uptr<Generator> generator_;
};

}
