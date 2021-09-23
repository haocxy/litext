#include "main_window.h"

#include <set>

#include <QMenuBar>
#include <QKeyEvent>
#include <QFileDialog>
#include <QInputDialog>
#include <QErrorMessage>

#include "core/font.h"
#include "core/time.h"
#include "core/logger.h"
#include "core/system.h"
#include "editor/editor.h"
#include "gui/text_area.h"
#include "gui/config.h"
#include "gui/text_area_config.h"

#include "editor_widget.h"
#include "text_area_widget.h"
#include "big_file_generator_widget.h"


namespace gui::qt
{


static const char *kFontFamilyTimes = "Times";
static const char *kFontFamilyYaHei = "Microsoft YaHei";

static void selectFont(font::FontContext &context, font::FontFile &fileTo, font::FontFace &faceTo) {
    static std::vector<std::string> GoodFontFamilies{ "Noto Sans SC", "Microsoft YaHei", "Noto Sans Mono CJK SC" };
    ElapsedTime elapsed;
    elapsed.start();

    const std::vector<fs::path> fontFiles = SystemUtil::fonts();

    auto bestIt = GoodFontFamilies.end();

    fs::path bestPath;
    long bestFace = -1;

    for (const fs::path &file : fontFiles) {
        font::FontFile fontFile(context, file);
        if (!fontFile) {
            continue;
        }

        for (long i = 0; i < fontFile.faceCount(); ++i) {
            font::FontFace face(fontFile, i);
            if (!face || face.isBold() || face.isItalic() || !face.isScalable()) {
                continue;
            }

            auto it = std::find(GoodFontFamilies.begin(), GoodFontFamilies.end(), face.familyName());
            if ((it != GoodFontFamilies.end())) {
                if (bestIt == GoodFontFamilies.end() || it < bestIt) {
                    bestIt = it;
                    bestPath = file;
                    bestFace = i;
                }
            }
        }
    }

    if (bestIt != GoodFontFamilies.end()) {
        fileTo = font::FontFile(context, bestPath);
        faceTo = font::FontFace(fileTo, bestFace);
        LOGI << "selectFont time usage: [" << elapsed.ms() << " ms]";
        return;
    }

    for (const fs::path &file : fontFiles) {
        font::FontFile fontFile(context, file);
        if (fontFile) {
            for (long i = 0; i < fontFile.faceCount(); ++i) {
                font::FontFace face(fontFile, i);
                if (face) {
                    fileTo = std::move(fontFile);
                    faceTo = std::move(face);
                    LOGI << "selectFont done without bad font, time usage: [" << elapsed.ms() << " ms]";
                    return;
                }
            }
        }
    }
}

static font::FontIndex selectFont()
{
    font::FontContext context;
    font::FontFile fontFile;
    font::FontFace fontFace;
    selectFont(context, fontFile, fontFace);
    if (fontFile && fontFace) {
        return font::FontIndex(fontFile.path(), fontFace.faceIndex());
    } else {
        return font::FontIndex();
    }
}


static void setupConfig(TextAreaConfig &c)
{
    HLayoutConfig hLayout;
    hLayout.setGap(2);
    hLayout.setPad(0);
    hLayout.setTabSize(4);
    hLayout.setWrapLine(true);

    c.setHLayout(hLayout);
    c.render().setWidthLimit(800);
    c.setShowLineNum(true);
    c.setLineNumOffset(1);

    FontOld f;
    FontInfoOld fi;
    fi.family = kFontFamilyYaHei;
    fi.size = 36;
    fi.bold = false;
    f.setFont(fi);
    c.setFont(f);
}

namespace prop
{
static const u8str isMax = "isMax";
static const u8str width = "width";
static const u8str height = "height";
static const u8str x = "x";
static const u8str y = "y";
static const u8str fontFile = "fontFile";
static const u8str fontFace = "fontFace";
}

MainWindow::MainWindow(Engine &engine, Config &config)
    : engine_(engine)
    , config_(config)
    , propRepo_(engine_.propDb(), "MainWindow")
{
    // 关闭后不删除
    setAttribute(Qt::WA_DeleteOnClose, false);

    setupConfig(config_.textAreaConfig());

    initMenuBar();

    initToolBar();

    restoreGeometry();

    u32str fontFile;
    i64 fontFace = 0;
    if (propRepo_.get(prop::fontFile, fontFile) && propRepo_.get(prop::fontFace, fontFace)) {
        config_.textAreaConfig().setFontIndex(font::FontIndex(fontFile, fontFace));
        LOGI << "got font from prop db";
    } else {
        const font::FontIndex fontIndex = selectFont();
        config_.textAreaConfig().setFontIndex(fontIndex);
        propRepo_.set(prop::fontFile, fontIndex.file().generic_u32string());
        propRepo_.set(prop::fontFace, fontIndex.faceIndex());
    }

    editorStack_ = new EditorStackWidget(engine_, this);
    setCentralWidget(editorStack_);
}

MainWindow::~MainWindow()
{
    saveGeometry();
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    editorStack_->closeAllDoc();
    e->accept();
}

void MainWindow::saveGeometry()
{
    if (isMaximized()) {
        propRepo_.set(prop::isMax, true);
    } else {
        propRepo_.set(prop::isMax, false);
        const QRect &geo = geometry();
        propRepo_.set(prop::width, geo.width());
        propRepo_.set(prop::height, geo.height());
        propRepo_.set(prop::x, geo.x());
        propRepo_.set(prop::y, geo.y());
    }
}

void MainWindow::restoreGeometry()
{
    bool isMax = false;
    if (propRepo_.get(prop::isMax, isMax)) {
        if (isMax) {
            return;
        }
    }

    int width = 800, height = 600;
    propRepo_.get(prop::width, width);
    propRepo_.get(prop::height, height);
    resize(width, height);

    // 如果获取位置信息失败则不做任何事情（默认的位置应该由GUI库决定）
    int x = 0, y = 0;
    if (propRepo_.get(prop::x, x) && propRepo_.get(prop::y, y)) {
        QRect geo = geometry();
        geo.moveTo(x, y);
        setGeometry(geo);
    }
}

void MainWindow::initMenuBar()
{
    QMenuBar *bar = menuBar();

    QMenu *fileMenu = bar->addMenu(tr("&File"));
    bind(fileMenu, tr("&Open"), &MainWindow::fileMenuOpenActionTriggered);
    bind(fileMenu, tr("&Quit"), &MainWindow::close);

    QMenu *viewMenu = bar->addMenu(tr("&View"));
    bind(viewMenu, tr("&GoLine"), &MainWindow::viewMenuGoLineActionTriggered);

    QMenu *testToolMenu = bar->addMenu(tr("&TestTool"));
    bind(testToolMenu, tr("&MakeBigFile"), &MainWindow::makeBigFileActionTriggered);
}

void MainWindow::initToolBar()
{
}

static RowN goToLineToRowOff(int goToLine)
{
    if (goToLine > 0) {
        return static_cast<RowN>(goToLine) - 1;
    } else if (goToLine < 0) {
        return goToLine;
    } else {
        return 0;
    }
}

void MainWindow::viewMenuGoLineActionTriggered()
{
    EditorWidget *curEditor = editorStack_->currentEditor();

    if (curEditor) {

        bool ok = false;

        const int goToLine = QInputDialog::getInt(
            this, tr("GoLine"), tr("Jump to line in doument"),
            curEditor->textAreaWidget().rowOffset() + 1,
            std::numeric_limits<int>::min(), std::numeric_limits<int>::max(),
            1, &ok);

        if (ok) {
            curEditor->jumpTo(goToLineToRowOff(goToLine));
        }
    }
}

void MainWindow::makeBigFileActionTriggered()
{
    BigFileGeneratorWidget *w = new BigFileGeneratorWidget;
    w->setAttribute(Qt::WA_DeleteOnClose);

    connect(w, &BigFileGeneratorWidget::shouldOpenGeneratedFile, this, [this](QString path) {
        openDocument(path.toStdU32String(), 0);
    });

    w->show();
}

void MainWindow::openDocument(const fs::path &file, RowN row)
{
    editorStack_->openDoc(file, row);
}

void MainWindow::fileMenuOpenActionTriggered()
{
    const std::u32string userHome = SystemUtil::userHome().generic_u32string();
    const QString defaultDir = QString::fromStdU32String(userHome);
    const QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open Document"), defaultDir);

    for (const QString &fileName : fileNames) {
        const fs::path path = fileName.toStdU16String();
        if (!path.empty()) {
            openDocument(path, 0);
        }
    }
}


}
