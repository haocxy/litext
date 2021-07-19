#include "editor_view_widget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "ruler_widget.h"
#include "text_area_widget.h"
#include "status_bar_widget.h"


namespace gui::qt
{


EditorViewWidget::EditorViewWidget(TextArea &textArea, QWidget *parent)
    : QWidget(parent)
{
    ruler_ = new RulerWidget(textArea);
    textArea_ = new TextAreaWidget;
    textArea_->bind(&textArea);
    vScrollBar_ = new QScrollBar;
    statusBar_ = new StatusBarWidget(textArea);

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setMargin(0);
    vlayout->setSpacing(0);

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->setMargin(0);
    hlayout->setSpacing(0);
    hlayout->addWidget(ruler_);
    hlayout->addWidget(textArea_);
    hlayout->addWidget(vScrollBar_);

    vlayout->addLayout(hlayout);
    vlayout->addWidget(statusBar_);

    setLayout(vlayout);
}

EditorViewWidget::~EditorViewWidget()
{
    ruler_ = nullptr;
    textArea_ = nullptr;
    vScrollBar_ = nullptr;
    statusBar_ = nullptr;
}


}
