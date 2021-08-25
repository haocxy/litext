#include "main_window.h"

#include <QMenuBar>
#include <QKeyEvent>
#include <QFileDialog>
#include <QInputDialog>
#include <QErrorMessage>

#include "core/system.h"
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
    bind(fileMenu, tr("&Open"), &MainWindow::fileMenuOpenActionTriggered);
    bind(fileMenu, tr("&Quit"), &MainWindow::close);

    QMenu *viewMenu = bar->addMenu(tr("&View"));
    bind(viewMenu, tr("&GoLine"), &MainWindow::viewMenuGoLineActionTriggered);
}

void MainWindow::initToolBar()
{
}

static RowN goToLineToRowOff(int goToLine)
{
    if (goToLine > 0) {
        return goToLine - 1;
    } else if (goToLine < 0) {
        return goToLine;
    } else {
        return 0;
    }
}

void MainWindow::viewMenuGoLineActionTriggered()
{
    if (editorWidget_) {
        const int goToLine = QInputDialog::getInt(this, tr("GoLine"), tr("Jump to line in doument"));
        editorWidget_->jumpTo(goToLineToRowOff(goToLine));
    }
}

void MainWindow::openDocument(const fs::path &file, RowN row)
{
    editorWidget_ = new EditorWidget(config_.textAreaConfig(), file, row);

    connect(editorWidget_, &EditorWidget::qtSigDocFatalError, this, [this](const QString &errmsg) {
        QErrorMessage::qtHandler()->showMessage(errmsg);
        setCentralWidget(nullptr);
        setWindowTitle(QString("notesharp"));
    });

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
        openDocument(path, 0);
    }
}


}
