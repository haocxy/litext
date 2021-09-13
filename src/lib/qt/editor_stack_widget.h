#pragma once

#include <map>

#include <QTabWidget>

#include "core/fs.h"

#include "gui/declare_engine.h"

#include "editor_widget.h"


struct GuiQtEditorCreatedInfo {

    GuiQtEditorCreatedInfo() {}

    GuiQtEditorCreatedInfo(const GuiQtEditorCreatedInfo &b)
        : editor(b.editor), absPath(b.absPath), row(b.row) {}

    GuiQtEditorCreatedInfo(GuiQtEditorCreatedInfo &&b)
        : editor(std::move(b.editor)), absPath(std::move(b.absPath)), row(b.row) {
        b.row = 0;
    }

    GuiQtEditorCreatedInfo &operator=(const GuiQtEditorCreatedInfo &b) {
        editor = b.editor;
        absPath = b.absPath;
        row = b.row;
        return *this;
    }

    GuiQtEditorCreatedInfo &operator=(GuiQtEditorCreatedInfo &&b) {
        editor = std::move(b.editor);
        absPath = std::move(b.absPath);
        row = b.row;
        b.row = 0;
        return *this;
    }

    std::shared_ptr<Editor> editor;
    fs::path absPath;
    RowN row = 0;
};

Q_DECLARE_METATYPE(GuiQtEditorCreatedInfo);


namespace gui::qt
{

class EditorStackWidget : public QTabWidget {
    Q_OBJECT
public:
    EditorStackWidget(Engine &engine, QWidget *parent = nullptr);

    virtual ~EditorStackWidget();

    void openDoc(const fs::path &file, RowN row);

    void closeAllDoc();

    EditorWidget *currentEditor();

private:
signals:
    void qtSigEditorCreated(GuiQtEditorCreatedInfo);

private:
    void closeDocByTabIndex(int tabIndex);

    void qtSlotEditorCreated(GuiQtEditorCreatedInfo info);

private:
    Engine &engine_;
    std::map<fs::path, std::vector<uptr<EditorWidget>>> editors_;
};

}


