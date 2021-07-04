#include "editor_view_widget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "line_num_bar.h"
#include "text_area_widget.h"
#include "view_status_bar.h"


EditorViewWidget::EditorViewWidget(View * view, QWidget * parent)
    : QWidget(parent)
{
    m_lineNumBar = new LineNumBarWidget(view);
    textArea_ = new TextAreaWidget(view);
	m_viewStatusBar = new ViewStatusBar(view);

	QVBoxLayout * vlayout = new QVBoxLayout;
	vlayout->setMargin(0);
	vlayout->setSpacing(0);

    QHBoxLayout * hlayout = new QHBoxLayout;
    hlayout->setMargin(0);
    hlayout->setSpacing(0);
    hlayout->addWidget(m_lineNumBar);
    hlayout->addWidget(textArea_);

	vlayout->addLayout(hlayout);
	vlayout->addWidget(m_viewStatusBar);

    setLayout(vlayout);
}

EditorViewWidget::~EditorViewWidget()
{
    m_lineNumBar = nullptr;
    textArea_ = nullptr;
}
