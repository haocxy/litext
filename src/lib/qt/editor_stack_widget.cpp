#include "editor_stack_widget.h"

#include "gui/engine.h"


namespace gui::qt
{

EditorStackWidget::EditorStackWidget(Engine &engine, QWidget *parent)
    : QTabWidget(parent)
    , engine_(engine) {

    setTabsClosable(true);

    connect(this, &EditorStackWidget::tabCloseRequested, this, &EditorStackWidget::closeDocByTabIndex);

    connect(this, &EditorStackWidget::tabBarDoubleClicked, this, [this](int tabIndex) {
        if (tabIndex >= 0) { // 根据Qt文档,负数是有含义的,必须判断
            closeDocByTabIndex(tabIndex);
        }
    });
}

EditorStackWidget::~EditorStackWidget()
{
}

void EditorStackWidget::openDoc(const fs::path &file, RowN row)
{
    const fs::path absPath = fs::absolute(file);
    auto it = editors_.find(absPath);
    if (it != editors_.end()) {
        if (!it->second.empty()) {
            setCurrentWidget(it->second.back().get());
            return;
        }
    }

    sptr<Editor> editor = engine_.editorManager().get(absPath);
    if (!editor) {
        return;
    }

    EditorWidget *editorWidget = new EditorWidget(engine_.config().textAreaConfig(), editor, row);

    addTab(editorWidget, QString::fromStdU32String(absPath.filename().generic_u32string()));

    setCurrentWidget(editorWidget);

    editors_[absPath].push_back(uptr<EditorWidget>(editorWidget));

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

void EditorStackWidget::closeDocByTabIndex(int tabIndex)
{
    EditorWidget *editorWidget = static_cast<EditorWidget *>(widget(tabIndex));
    removeTab(tabIndex);
    const fs::path absPath = fs::absolute(editorWidget->document().path());
    editors_.erase(absPath);
}

}
