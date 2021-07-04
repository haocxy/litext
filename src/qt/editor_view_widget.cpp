#include "editor_view_widget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "ruler_widget.h"
#include "text_area_widget.h"
#include "status_bar_widget.h"


EditorViewWidget::EditorViewWidget(TextAreaView * view, QWidget * parent)
    : QWidget(parent)
{
    ruler_ = new RulerWidget(view);
    textArea_ = new TextAreaWidget(view);
	statusBar_ = new StatusBarWidget(view);

	QVBoxLayout * vlayout = new QVBoxLayout;
	vlayout->setMargin(0);
	vlayout->setSpacing(0);

    QHBoxLayout * hlayout = new QHBoxLayout;
    hlayout->setMargin(0);
    hlayout->setSpacing(0);
    hlayout->addWidget(ruler_);
    hlayout->addWidget(textArea_);

	vlayout->addLayout(hlayout);
	vlayout->addWidget(statusBar_);

    setLayout(vlayout);
}

EditorViewWidget::~EditorViewWidget()
{
    ruler_ = nullptr;
    textArea_ = nullptr;
    statusBar_ = nullptr;
}
