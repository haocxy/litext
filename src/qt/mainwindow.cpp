#include "mainwindow.h"

#include "view/view.h"
#include "view/view_config.h"
#include "editor/editor.h"
#include "doc/simple_doc.h"
#include "editor_view_widget.h"

const char *kFontFamilyTimes = "Times";
const char *kFontFamilyYaHei = "Microsoft YaHei";

static void setupConfig(view::Config &c)
{
    c.setLineHeightFactor(1.f);
    c.setHGap(2);
    c.setHMargin(0);
    c.setTabSize(4);
    c.setWrapLine(true);
    c.setShowLineNum(true);
    c.setLineNumOffset(1);

    view::Font &f = c.rfont();
    view::FontInfo fi;
    fi.family = kFontFamilyYaHei;
    fi.size = 22;
    fi.bold = false;

    f.setFont(fi);
}

MainWindow::MainWindow(QWidget * parent)
    : QMainWindow(parent)
{
    m_viewConfig = new view::Config();
    setupConfig(*m_viewConfig);
    m_doc = new SimpleDoc;
    m_doc->LoadFromFile(R"(../nocode/demo/a.txt)");
    
    m_editor = new Editor(m_doc);
    m_view = new View(m_editor, m_viewConfig);
    m_editorViewWidget = new EditorViewWidget(m_view);


    this->setCentralWidget(m_editorViewWidget);
}


MainWindow::~MainWindow()
{
    delete m_viewConfig;
    m_viewConfig = nullptr;

    delete m_doc;
    m_doc = nullptr;

    delete m_view;
    m_view = nullptr;

    delete m_editor;
    m_editor = nullptr;

    delete m_editorViewWidget;
    m_editorViewWidget = nullptr;
}

void MainWindow::initMenuBar()
{

}

