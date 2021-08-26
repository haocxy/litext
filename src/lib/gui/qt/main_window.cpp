#include "main_window.h"

#include <QMenuBar>
#include <QKeyEvent>
#include <QFileDialog>
#include <QInputDialog>
#include <QErrorMessage>

#include "core/system.h"
#include "doc/simple_doc.h"
#include "editor/editor.h"
#include "gui/text_area.h"
#include "gui/config.h"
#include "gui/text_area_config.h"

#include "editor_widget.h"
#include "text_area_widget.h"


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
    fi.size = 16;
    fi.bold = false;
    f.setFont(fi);
    c.setFont(f);
}

namespace prop
{
static const std::string_view width = "width";
static const std::string_view height = "height";
static const std::string_view x = "x";
static const std::string_view y = "y";
}

MainWindow::MainWindow(Engine &engine, Config &config)
    : engine_(engine)
    , config_(config)
    , propRepo_(engine_.propDb(), "MainWindow")
{
    setupConfig(config_.textAreaConfig());

    initMenuBar();

    initToolBar();

    // 尝试获取之前保存到数据库中的宽度和高度，
    // 如果失败则使用默认值
    int width = 800, height = 600;
    propRepo_.get(prop::width, width);
    propRepo_.get(prop::height, height);
    resize(width, height);

    // 尝试获取之前保存到数据库中的位置
    // 如果失败则不做任何事情（默认的位置应该由GUI库决定）
    int x = 0, y = 0;
    if (propRepo_.get(prop::x, x) && propRepo_.get(prop::y, y)) {
        move(x, y);
    }
}

MainWindow::~MainWindow()
{
    const QSize sz = size();
    propRepo_.set(prop::width, sz.width());
    propRepo_.set(prop::height, sz.height());

    const QPoint ps = pos();
    propRepo_.set(prop::x, ps.x());
    propRepo_.set(prop::y, ps.y());

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
        return static_cast<RowN>(goToLine) - 1;
    } else if (goToLine < 0) {
        return goToLine;
    } else {
        return 0;
    }
}

void MainWindow::viewMenuGoLineActionTriggered()
{
    EditorWidget *curEditor = editorWidget_;

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