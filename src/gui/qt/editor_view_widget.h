#pragma once

#include <QWidget>
#include <QScrollBar>

#include "core/signal.h"
#include "doc/part_loaded_event.h"
#include "gui/declare_text_area.h"
#include "declare_ruler_widget.h"
#include "declare_text_area_widget.h"
#include "declare_status_bar_widget.h"


namespace gui::qt
{


class EditorViewWidget : public QWidget {
    Q_OBJECT
public:
    EditorViewWidget(TextArea &textArea, QWidget *parent = nullptr);

    virtual ~EditorViewWidget();

    TextAreaWidget *textAreaWidget() {
        return textArea_;
    }

private:
signals:
    void qtSigPartLoaded(double loadedPercent);

private slots:
    void qtSlotPartLoaded(double loadedPercent);

private:
    RulerWidget *ruler_ = nullptr;
    TextAreaWidget *textArea_ = nullptr;
    QScrollBar *vScrollBar_ = nullptr;
    StatusBarWidget *statusBar_ = nullptr;
    Slot slotPartLoaded_;
};


}
