#include "main_window.h"

#include <QKeyEvent>

#include "gui/text_area.h"
#include "gui/text_area_config.h"
#include "editor/editor.h"
#include "doc/simple_doc.h"
#include "editor_view_widget.h"


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

    Font &f = c.rfont();
    FontInfo fi;
    fi.family = kFontFamilyYaHei;
    fi.size = 22;
    fi.bold = false;
    f.setFont(fi);
}

MainWindow::MainWindow(TextAreaConfig &textAreaConfig, const fs::path &filePath)
    : textAreaConfig_(textAreaConfig)
{
    setupConfig(textAreaConfig_);

    editorWidget_ = new EditorViewWidget(strandPool_, textAreaConfig, filePath);

    setCentralWidget(editorWidget_);
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

}


}
