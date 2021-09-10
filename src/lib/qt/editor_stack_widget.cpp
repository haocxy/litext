#include "editor_stack_widget.h"

#include "gui/engine.h"


namespace gui::qt
{

EditorStackWidget::EditorStackWidget(Engine &engine, QWidget *parent)
    : QTabWidget(parent)
    , engine_(engine) {

}

EditorStackWidget::~EditorStackWidget()
{
}

void EditorStackWidget::openDoc(const fs::path &file, RowN row)
{
    EditorWidget *editorWidget = new EditorWidget(engine_.config().textAreaConfig(), file, row);

    addTab(editorWidget, QString::fromStdU32String(file.filename().generic_u32string()));

    editors_[file] = uptr<EditorWidget>(editorWidget);

    editorWidget = nullptr;
}

EditorWidget *EditorStackWidget::currentEditor()
{
    return static_cast<EditorWidget *>(currentWidget());
}

}
