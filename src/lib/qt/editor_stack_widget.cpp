#include "editor_stack_widget.h"

#include "core/logger.h"
#include "gui/engine.h"


namespace gui::qt
{

EditorStackWidget::EditorStackWidget(Engine &engine, QWidget *parent)
    : QTabWidget(parent)
    , engine_(engine) {

    qRegisterMetaType<GuiQtEditorCreatedInfo>("GuiQtEditorCreatedInfo");

    setTabsClosable(true);
    setMovable(true);
    setUsesScrollButtons(true);

    connect(this, &EditorStackWidget::tabCloseRequested, this, &EditorStackWidget::closeDocByTabIndex);

    connect(this, &EditorStackWidget::tabBarDoubleClicked, this, [this](int tabIndex) {
        if (tabIndex >= 0) { // 根据Qt文档,负数是有含义的,必须判断
            closeDocByTabIndex(tabIndex);
        }
    });

    connect(this, &EditorStackWidget::qtSigEditorCreated, this, &EditorStackWidget::qtSlotEditorCreated);

    engine_.editorManager().registerReciver(this);
}

EditorStackWidget::~EditorStackWidget()
{
    engine_.editorManager().removeReciver(this);
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

    engine_.editorManager().asyncGet(this, absPath, [this, absPath, row](std::shared_ptr<Editor> editor) {
        GuiQtEditorCreatedInfo info;
        info.editor = editor;
        info.absPath = absPath;
        info.row = row;
        emit qtSigEditorCreated(info);
    });
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

void EditorStackWidget::qtSlotEditorCreated(GuiQtEditorCreatedInfo info)
{
    EditorWidget *editorWidget = new EditorWidget(engine_.config().textAreaConfig(), info.editor, info.row);

    addTab(editorWidget, QString::fromStdU32String(info.absPath.filename().generic_u32string()));

    setCurrentWidget(editorWidget);

    editors_[info.absPath].push_back(uptr<EditorWidget>(editorWidget));

    editorWidget = nullptr;
}

}
