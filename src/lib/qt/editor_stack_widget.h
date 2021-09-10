#pragma once

#include <map>

#include <QTabWidget>

#include "core/fs.h"

#include "gui/declare_engine.h"

#include "editor_widget.h"



namespace gui::qt
{

class EditorStackWidget : public QTabWidget {
    Q_OBJECT
public:
    EditorStackWidget(Engine &engine, QWidget *parent = nullptr);

    virtual ~EditorStackWidget();

    void openDoc(const fs::path &file, RowN row);

    EditorWidget *currentEditor();

private:
    Engine &engine_;
    std::map<fs::path, uptr<EditorWidget>> editors_;
};

}
