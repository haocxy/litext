#include "mainwindow.h"

#include "gui/text_area.h"
#include "gui/text_area_config.h"
#include "editor/editor.h"
#include "doc/simple_doc.h"
#include "doc/async_doc_server.h"
#include "editor_view_widget.h"
#include "object_worker.h"


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

MainWindow::MainWindow(fs::path filePath, QWidget *parent)
    : QMainWindow(parent)
{
    m_viewConfig = new TextAreaConfig();
    setupConfig(*m_viewConfig);
    m_doc = new SimpleDoc;
    m_doc->LoadFromFile(filePath.generic_string());

    m_objWorker = new ObjectWorker(m_receiver);
    m_docServer = new doc::AsyncDocServer(*m_objWorker, filePath);
    m_editor = new Editor(m_doc, *m_docServer);
    m_view = new TextArea(m_editor, m_viewConfig);
    m_editorViewWidget = new EditorViewWidget(*m_view);

    this->setCentralWidget(m_editorViewWidget);
}


MainWindow::~MainWindow()
{
    delete m_objWorker;
    m_objWorker = nullptr;

    delete m_docServer;
    m_docServer = nullptr;

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


}
