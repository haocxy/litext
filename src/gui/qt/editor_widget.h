#pragma once

#include <QWidget>
#include <QScrollBar>

#include "core/fs.h"
#include "core/sigconns.h"
#include "doc/part_loaded_event.h"
#include "doc/simple_doc.h"
#include "editor/editor.h"
#include "gui/text_area.h"
#include "gui/declare_text_area_config.h"
#include "declare_ruler_widget.h"
#include "declare_text_area_widget.h"
#include "declare_status_bar_widget.h"


namespace gui::qt
{


class EditorWidget : public QWidget {
    Q_OBJECT
public:
    EditorWidget(const TextAreaConfig &textAreaConfig, const fs::path &file);

    virtual ~EditorWidget();

private:
signals:
    void qtSigPartLoaded(double loadedPercent);

private slots:
    void qtSlotPartLoaded(double loadedPercent);

private:
    const fs::path file_;
    SimpleDoc doc_;
    Editor editor_;
    TextArea textArea_;
    RulerWidget *ruler_ = nullptr;
    TextAreaWidget *textAreaWidget_ = nullptr;
    QScrollBar *vScrollBar_ = nullptr;
    StatusBarWidget *statusBar_ = nullptr;
    SigConns sigConns_;
};


}
