#include "main_window.h"

#include <QKeyEvent>
#include <QMenuBar>
#include <QFileDialog>

#include "core/system_util.h"
#include "gui/text_area.h"
#include "gui/config.h"
#include "gui/text_area_config.h"
#include "editor/editor.h"
#include "doc/simple_doc.h"
#include "editor_widget.h"


namespace gui::qt
{


static const char *kFontFamilyTimes = "Times";
static const char *kFontFamilyYaHei = "Microsoft YaHei";

static void setupConfig(TextAreaConfig &c)
{
    c.setLineHeightFactor(1.f);
    c.setHGap(2);
    c.setHMargin(0);
    c.setTabSize(4);
    c.setWrapLine(true);
    c.setShowLineNum(true);
    c.setLineNumOffset(1);

    Font f;
    FontInfo fi;
    fi.family = kFontFamilyYaHei;
    fi.size = 22;
    fi.bold = false;
    f.setFont(fi);
    c.setFont(f);
}

MainWindow::MainWindow(Config &config)
    : config_(config)
{
    setupConfig(config_.textAreaConfig());

    initMenuBar();

    initToolBar();

    resize(800, 600);
}

MainWindow::MainWindow(Config &config, const fs::path &filePath)
    : MainWindow(config)
{
    openDocument(filePath);
}

MainWindow::~MainWindow()
{
    editorWidget_ = nullptr;
}

void MainWindow::keyReleaseEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape) {
        close();
    } else {
        QMainWindow::keyReleaseEvent(e);
    }
}

void MainWindow::initMenuBar()
{
    QMenuBar *bar = menuBar();

    QMenu *fileMenu = bar->addMenu(tr("&File"));
    QAction *openAction = fileMenu->addAction(tr("&Open"));
    connect(openAction, &QAction::triggered, this, &MainWindow::fileMenuOpenActionTriggered);
    QAction *quitAction = fileMenu->addAction(tr("&Quit"));
    connect(quitAction, &QAction::triggered, this, &MainWindow::close);
}

void MainWindow::initToolBar()
{
}

void MainWindow::openDocument(const fs::path &file)
{
    editorWidget_ = new EditorWidget(config_.textAreaConfig(), file);

    setCentralWidget(editorWidget_);

    setWindowTitle(QString("notesharp - ") + QString::fromStdU16String(file.generic_u16string()));
}

void MainWindow::fileMenuOpenActionTriggered()
{
    const std::u16string userHome = SystemUtil::userHome().generic_u16string();
    const QString defaultDir = QString::fromStdU16String(userHome);
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Open Document"), defaultDir);
    const fs::path path = fileName.toStdU16String();

    if (!path.empty()) {
        openDocument(path);
    }
}


}
