#include "editor_stack_widget.h"

#include "gui/engine.h"


namespace gui::qt
{

EditorStackWidget::EditorStackWidget(Engine &engine, QWidget *parent)
    : QTabWidget(parent)
    , engine_(engine) {

    setTabsClosable(true);
}

EditorStackWidget::~EditorStackWidget()
{
}

void EditorStackWidget::openDoc(const fs::path &file, RowN row)
{
    auto it = editors_.find(file);
    if (it != editors_.end()) {
        setCurrentWidget(it->second.get());
        return;
    }

    Editor *editor = engine_.editorManager().get(file);
    if (!editor) {
        return;
    }

    EditorWidget *editorWidget = new EditorWidget(engine_.config().textAreaConfig(), *editor, row);

    addTab(editorWidget, QString::fromStdU32String(file.filename().generic_u32string()));

    setCurrentWidget(editorWidget);

    editors_[file] = uptr<EditorWidget>(editorWidget);

    editorWidget = nullptr;
}

void EditorStackWidget::closeAllDoc()
{
    clear();
    editors_.clear();
}

EditorWidget *EditorStackWidget::currentEditor()
{
    return static_cast<EditorWidget *>(currentWidget());
}

}
