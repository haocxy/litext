#include "editor_widget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "gui/text_area.h"
#include "editor/editor.h"
#include "ruler_widget.h"
#include "text_area_widget.h"
#include "editor_status_bar_widget.h"


namespace gui::qt
{

EditorWidget::EditorWidget(const TextAreaConfig &textAreaConfig, sptr<Editor> editor, RowN row)
    : editor_(editor)
    , textArea_(*editor_, textAreaConfig)
{
    assert(editor_);

    doc::Document &document = editor_->document();

    ruler_ = new RulerWidget(textArea_);

    textAreaWidget_ = new TextAreaWidget(textArea_);

    vScrollBar_ = new TextAreaScrollBar(textArea_);

    statbar_ = new EditorStatusBarWidget(textArea_);

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setMargin(0);
    vlayout->setSpacing(0);

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->setMargin(0);
    hlayout->setSpacing(0);
    hlayout->addWidget(ruler_);
    hlayout->addWidget(textAreaWidget_);
    hlayout->addWidget(vScrollBar_);

    vlayout->addLayout(hlayout);
    vlayout->addWidget(statbar_);

    setLayout(vlayout);

    textArea_.open(Size(textAreaWidget_->width(), textAreaWidget_->height()), row);
}

EditorWidget::~EditorWidget()
{
    sigConns_.clear();

    ruler_ = nullptr;
    textAreaWidget_ = nullptr;
    vScrollBar_->disconnectAllSignals();
    vScrollBar_ = nullptr;
    statbar_ = nullptr;
}

const TextAreaWidget &EditorWidget::textAreaWidget() const
{
    return *textAreaWidget_;
}

const doc::Document &EditorWidget::document() const
{
    return editor_->document();
}

void EditorWidget::jumpTo(RowN row)
{
    textArea_.jumpTo(row);
}

void EditorWidget::pageUp()
{
    textArea_.movePage(TextArea::Dir::Up);
}

void EditorWidget::pageDown()
{
    textArea_.movePage(TextArea::Dir::Down);
}

QString EditorWidget::docErrToStr(doc::DocError err) const
{
    switch (err) {
    case doc::DocError::RowTooBig:
        return tr("row too big");
    case doc::DocError::UnsupportedCharset:
        return tr("unsupported charset");
    default:
        return tr("unknown error");
    }
}

}
