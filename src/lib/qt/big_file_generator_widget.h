#pragma once

#include <thread>
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

protected:
    virtual void mousePressEvent(QMouseEvent *e) override;

private:
signals:
    void generateProgress(int percent);

    void generateDone();

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

    private:
        BigFileGeneratorWidget &gui_;
        GenerateParam param_;
        std::thread thread_;
    };

private:
    Ui::BigFileGenerator *ui_ = nullptr;
    QWidget *lastActRangeEditor_ = nullptr;
    uptr<Generator> generator_;
};

}
