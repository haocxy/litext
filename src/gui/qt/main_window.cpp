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

MainWindow::MainWindow(TextAreaConfig &textAreaConfig, const fs::path &filePath, QWidget *parent)
    : QMainWindow(parent)
    , textAreaConfig_(textAreaConfig)
{
    setupConfig(textAreaConfig_);
    m_doc = new SimpleDoc;
    m_doc->LoadFromFile(filePath.generic_string());

    editor_ = new Editor(m_doc, strandPool_, filePath, guiStrand_);
    m_view = new TextArea(strandPool_, editor_, &textAreaConfig_);
    m_editorViewWidget = new EditorViewWidget(*m_view);

    setCentralWidget(m_editorViewWidget);
}


MainWindow::~MainWindow()
{
    delete m_doc;
    m_doc = nullptr;

    delete m_view;
    m_view = nullptr;

    delete editor_;
    editor_ = nullptr;

    delete m_editorViewWidget;
    m_editorViewWidget = nullptr;
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
