#include "editor_view_widget.h"

#include <QHBoxLayout>

#include "line_num_bar.h"
#include "textpad.h"

EditorViewWidget::EditorViewWidget(View * view, QWidget * parent)
    : QWidget(parent)
{
    m_lineNumBar = new LineNumBar(view, this);
    m_textPad = new TextPad(view, this);

    QHBoxLayout * layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_lineNumBar);
    layout->addWidget(m_textPad);

    setLayout(layout);
}

EditorViewWidget::~EditorViewWidget()
{
    m_lineNumBar = nullptr;
    m_textPad = nullptr;
}
